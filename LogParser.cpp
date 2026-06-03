//
// Created by c4ner on 6/2/2026.
//

#include "LogParser.h"
#include <fstream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <string_view>


using namespace std;

LogParser::LogParser() = default;
LogParser::~LogParser() = default;


void LogParser::runParser(const std::string &filePath) {
        auto start = std::chrono::high_resolution_clock::now();

        unsigned int numThreads = std::thread::hardware_concurrency();
            if (numThreads == 0) {
                numThreads = 4;
            }
        std::vector<thread> threads;
        threads.emplace_back(&LogParser::Reader, this, filePath);
        cout << "DEBUG: start Reader threads" << endl;

        for (unsigned int i = 0; i < numThreads; i++) {
            threads.emplace_back (&LogParser::Consumer_log, this); //&ИмяКласса::ИмяМетода, + указатель на сам объект (this)
            cout << "DEBUG: start Consumer threads" << endl;
        }
        for (auto & thread : threads)
        {
            thread.join();
        }
    printResults();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    cout << "Lead time(parsing): " << duration.count() << " microseconds" << endl;
}

void LogParser::printResults() const {
    vector<pair<string, size_t>> results;
    for (const auto &item : error_) {
        results.emplace_back(item);
    }
    ranges::sort(results, [](const pair<string, size_t> &a, const pair<string, size_t> &b) {
                     return a.second > b.second;
                 });
    for (auto &[fst, snd] : results) {
        cout << fst << " " << snd << endl;
    }
}


void LogParser::Reader(const std::string &filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Unable to open file " << filePath << endl;
        exit(1);
    }
    cout << "DEBUG: File is open. " << endl;
    string line;
    std::vector<std::string> batch;
    batch.reserve(BATCH_SIZE);
    while (getline(file, line)) {
        batch.emplace_back(line);
        if (batch.size() >= BATCH_SIZE) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                batches_.push(std::move(batch));
            }
            cv_.notify_one();
            batch.clear();
            batch.reserve(BATCH_SIZE);
        }
    }


    if (!batch.empty()) {
        std::lock_guard<std::mutex> lock(mutex_);
        batches_.push(std::move(batch));
        cv_.notify_one();
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        isDone_ = true;
    }
    cv_.notify_all();
}

void LogParser::Consumer_log() {
    try {
        std::unordered_map<std::string, std::size_t> local_error;
        while (true) {
            std::vector<std::string> batch;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]() {return !batches_.empty() || isDone_; }); //wait until queue == empty OR Rider isDone = true
                if (batches_.empty() && isDone_) {
                    std::lock_guard<std::mutex> mlock(mapMutex_);
                    for (const auto&[fst, snd] : local_error) {
                        error_[fst] += snd;
                    }
                    return;
                }
                batch = std::move(batches_.front());
                batches_.pop();
                lock.unlock();
            }; //mutex

            //.log struct: [дата время] УРОВЕНЬ: Сообщение
            //example: [2026-06-02 20:45:01] ERROR: Database connection timeout
            for (const auto& currLine : batch) {
                size_t startPos = currLine.find("] ");
                if (startPos == std::string::npos) {
                    continue;
                }
                startPos += 2;
                size_t endPos = currLine.find(':', startPos);
                if (endPos == std::string::npos) { //NOLINT
                    continue;
                }
                string_view errorLevel = string_view(currLine).substr(startPos, endPos - startPos); //23 index + : on 28 index, 28-23 = word length
                local_error[static_cast<std::string>(errorLevel)]++;
            }
        }
    }catch (const std::exception &e) {
        cerr << e.what() << endl;
    }
}


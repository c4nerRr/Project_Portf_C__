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

        for (int i = 0; i < numThreads; i++) {
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
    while (getline(file, line)) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            lines_.push(std::move(line));
        }
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
        while (true) {
            string currLine;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this]() {return !lines_.empty() || isDone_; }); //wait until queue == empty OR Rider isDone = true
                if (lines_.empty() && isDone_) {
                    return;
                }
                currLine = std::move(lines_.front());
                lines_.pop();
                lock.unlock();
            }; //mutex
            //.log struct: [дата время] УРОВЕНЬ: Сообщение
            //example: [2026-06-02 20:45:01] ERROR: Database connection timeout
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
            {
                std::lock_guard<std::mutex> lock2(mapMutex_);
                error_[static_cast<string>(errorLevel)]++;
            };
        }
    }catch (const std::exception &e) {
        cerr << e.what() << endl;
    }
}


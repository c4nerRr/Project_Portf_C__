//
// Created by c4ner on 6/2/2026.
//

#include "LogParser.h"
#include "DataTypes.h"
#include <fstream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <algorithm>
using namespace std;

LogParser::LogParser() = default;


LogParser::~LogParser() {
    Clear();
}

void LogParser::from_main_to_main(const std::string &filePath) {
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<thread> threads;

        threads.emplace_back(&LogParser::Reader, this, filePath);
        cout << "DEBUG: start Reader threads" << endl;

        for (int i = 0; i < 3; i++) {
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

    string line;
    while (getline(file, line)) {
        std::lock_guard<std::mutex> lock(mutex_);
        lines_.push_back(line);
    }
    isDone_ = true;
}

void LogParser::Consumer_log() {
    do {
        string currLine;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (lines_.empty()) {
                continue;
            }
            currLine = lines_.back();
            lines_.pop_back();
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
        string errorLevel = currLine.substr(startPos, endPos - startPos); //23 index + : on 28 index, 28-23 = word length
        {
            std::lock_guard<std::mutex> lock2(mutex_);
            error_[errorLevel]++;
        };
    }while (!isDone_ || !lines_.empty());
}

void LogParser::Clear() {
}

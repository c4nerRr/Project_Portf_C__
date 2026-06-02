//
// Created by c4ner on 6/2/2026.
//

#include "LogParser.h"
#include "DataTypes.h"
#include <fstream>
#include <mutex>
#include <thread>
#include <string_view>
#include <unordered_map>
#include <chrono>
#include <iostream>
using namespace std;

LogParser::LogParser() {
}


LogParser::~LogParser() {
    Clear();
}

void LogParser::from_main_to_main(const std::string &filePath) {
        std::vector<thread> threads;

        threads.emplace_back(&LogParser::Reader, this, filePath);
        cout << "DEBUG: start Reader threads" << endl;

        for (int i = 0; i < 3; i++) {
            threads.emplace_back (&LogParser::Consumer_log, this); //&ИмяКласса::ИмяМетода, + указатель на сам объект (this)
            cout << "DEBUG: start Consumer threads" << endl;
        }

        for (int i = 0; i < threads.size(); i++)
        {
            threads[i].join();
        }
}

void LogParser::printResults() {
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

        size_t startPos = currLine.find("] ") + 2;
        if (startPos == std::string::npos) {
            continue;
        }
        size_t endPos = currLine.find(":", startPos);
        if (startPos == std::string::npos) { //NOLINT
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

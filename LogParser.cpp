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
        //std::thread t1(Reader);
        Reader(filePath);

        std::vector<thread> threads;
        for (int i = 0; i<3; i++) {
            threads.emplace_back (&LogParser::Consumer_log, this); //&ИмяКласса::ИмяМетода, + указатель на сам объект (this)
        }
        //Consumer_log();
}

void LogParser::printResults() {
}


void LogParser::Reader(const std::string &filePath) { //порциями данные из файла.
    if (ifstream file(filePath); !file.is_open()) {
        cerr << "Unable to open file " << filePath << endl;
        exit(1);
    }


}

void LogParser::Consumer_log() {
}

void LogParser::Clear() {
}

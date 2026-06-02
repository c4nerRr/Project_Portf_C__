//
// Created by c4ner on 6/2/2026.
//

#ifndef PROJECT_PORTF_C___LOGPARSER_H
#define PROJECT_PORTF_C___LOGPARSER_H
#include <atomic>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <condition_variable>


class LogParser {
    public:
    LogParser();
    ~LogParser();
    void runParser(const std::string &filePath);
    void printResults() const;
    private:

    std::atomic<bool> isDone_ = false;
    std::queue<std::string> lines_;
    std::unordered_map<std::string, std::size_t> error_;
    std::mutex mutex_;
    std::mutex mapMutex_;
    std::condition_variable cv_;

    void Reader(const std::string& filePath);

    void Consumer_log();
    void Clear();
};


#endif //PROJECT_PORTF_C___LOGPARSER_H
//
// Created by c4ner on 6/2/2026.
//

#ifndef PROJECT_PORTF_C___LOGPARSER_H
#define PROJECT_PORTF_C___LOGPARSER_H
#include <atomic>
#include <string>
#include <unordered_map>
#include <vector>


class LogParser {
    public:
    LogParser();
    ~LogParser();
    void from_main_to_main(const std::string &filePath);
    void printResults() const;
    private:

    std::atomic<bool> isDone_ = false; //потоков много чтобы функции не дрались за флажки. атомарный флажок.
    std::vector<std::string> lines_; //склад прочитаных строчек
    std::unordered_map<std::string, std::size_t> error_; //ошибки, кол-во
    std::mutex mutex_; //не даст потокам одновременно писать данные в очередь или хешку

    void Reader(const std::string& filePath);

    void Consumer_log();
    void Clear();
};


#endif //PROJECT_PORTF_C___LOGPARSER_H
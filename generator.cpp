//
// Created by c4ner on 6/3/2026.
//
#include <ctime>
#include <fstream>
#include <string>
#include <cstdlib>
#include <iostream>

int main() {
    std::srand(std::time(nullptr)); //NOLINT
    std::ofstream outFile;
    outFile.open("../test.log");
    if (!outFile.is_open()) {
        std::cerr << "Unable to open file " << std::endl;
        return 1;
    }
    std::string error[] = {"INFO", "WARNING", "ERROR", "FATAL"};
    for (int i = 0; i < 10000000; i++) {
        const int randNum = rand() % std::size(error);
        outFile << "[2026-06-03 12:00:00] " << error[randNum] << ": Some random generic message\n";
    }
    outFile.close();
}

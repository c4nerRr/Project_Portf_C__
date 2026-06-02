#include <iostream>

#include "LogParser.h"

int main() {
    LogParser logParser;
    logParser.runParser("../test.log");
    return 0;
}

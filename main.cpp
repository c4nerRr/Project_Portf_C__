#include <iostream>

#include "LogParser.h"

int main() {
    LogParser logParser;
    logParser.from_main_to_main("../test.log");
    return 0;
}

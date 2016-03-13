#include <cstdio>
#include <fstream>
#include <iostream>
#include <chrono>

#include "json_parser.hpp"

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "[usage] %s [json file]\n", argv[0]);
        return 1;
    }

    std::string input;
    std::string line;
    std::ifstream stream(argv[1]);

    if(!stream) {
        fprintf(stderr, "cannot open file: %s\n", argv[0]);
        return 1;
    }

    while(std::getline(stream, line)) {
        input += line;
    }

    auto start = std::chrono::system_clock::now();

    auto p = aquarius::Parser<json::json>()(input.begin(), input.end());

    auto stop = std::chrono::system_clock::now();

    if(!static_cast<bool>(p)) {
        fprintf(stderr, "parse error\n%s\n", input.c_str());

        return 1;
    }

    auto interval = stop - start;
    std::cout << "time:" <<
    std::chrono::duration_cast<std::chrono::milliseconds>(interval).count() << "[ms]" << std::endl;

    return 0;
}



#pragma once
#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <string>
#include <vector>

class CommandProcessor{
public:
    static CommandProcessor& getInstance();
    static void initialize();
    static void destroy();

    std::vector<std::string> tokenize(const std::string &command, char delimiter);

private:
    CommandProcessor() = default;
    CommandProcessor(const CommandProcessor&) = delete;
    CommandProcessor& operator=(const CommandProcessor&) = delete;
    static CommandProcessor* sharedInstance;
};

#endif
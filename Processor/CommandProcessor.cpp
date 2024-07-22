#include <sstream>

#include "CommandProcessor.h"

CommandProcessor* CommandProcessor::sharedInstance = nullptr;

CommandProcessor& CommandProcessor::getInstance(){
    if (!sharedInstance)
        initialize();

    return *sharedInstance;
}

void CommandProcessor::initialize(){
    if (!sharedInstance)
        sharedInstance = new CommandProcessor();
}

void CommandProcessor::destroy(){
    if (sharedInstance){
        delete sharedInstance;
        sharedInstance = nullptr;
    }
}

std::vector<std::string> CommandProcessor::tokenize(const std::string &command, char delimiter){
    std::vector<std::string> tokens;
    std::stringstream ss(command);
    std::string token;

    while (std::getline(ss, token, delimiter))
        tokens.push_back(token);

    return tokens;
}

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <random>

#include "Process.h"
#include "../Command/PrintCommand.h"
#include "../UI/UI_Manager.h"

Process::Process(int pid, const std::string &name, RequirementFlags requirementFlags,
                const std::string &timestamp, int minInstructions, int maxInstructions, int minMem, int maxMem)
    : pid(pid), name(name), requirementFlags(requirementFlags),timestamp(timestamp),
    minInstructions(minInstructions), maxInstructions(maxInstructions),
    minMem(minMem), maxMem(maxMem){

        currentState = Process::READY;
        cpuCoreID = -1;
        commandCounter = 0;
        currInstruction = 0;
        generateRandomInstruction(minInstructions, maxInstructions);
        generateRandomMemReq(minMem, maxMem);

        this->maxInstructions = numInstruction - 1;
}

void Process::addCommand(ICommand::CommandType commandType){
    switch (commandType) {
        case ICommand::PRINT:
            commandList.push_back(std::make_shared<PrintCommand>(pid, "Hello WOrld"));
            break;
        // can add more
        default:
            break;
    }
}

void Process::executeCurrentCommand() const{
    if (commandCounter < static_cast<int>(commandList.size())){
        commandList[commandCounter]->execute();
    }
}

void Process::moveToNextLine(){
    if (commandCounter < static_cast<int>(commandList.size())){
        commandCounter++;
        if (commandCounter == static_cast<int>(commandList.size())){
            currentState = FINISHED;
        }
    }
}

void Process::executeTask(){
    for(int i = currInstruction; i < numInstruction; i++){
        currInstruction = i;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    currentState = Process::FINISHED;
}

void Process::executeTask(int quantumCycles){
    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::milliseconds(quantumCycles * 1000);

    for (int i = currInstruction; i < numInstruction; ++i){
        currInstruction = i;

        if (std::chrono::steady_clock::now() >= endTime)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    currentState = (currInstruction >= maxInstructions) ? Process::FINISHED : Process::RUNNING;
}

void Process::generateRandomInstruction(int min, int max){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);

    numInstruction = dis(gen);
}

void Process::generateRandomMemReq(int minMem, int maxMem){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minMem, maxMem);

    memoryRequired = dis(gen);
}

int Process::getPID() const{
    return pid;
}

int Process::getCommandCounter() const{
    return commandCounter;
}

std::string Process::getName() const{
    return name;
}

std::string Process::getTimestamp() const{
    return timestamp;
}

int Process::getCpuCoreID() const{
    return cpuCoreID;
};

int Process::getMinInstructions() const{
    return minInstructions;
};

int Process::getMaxInstructions() const{
    return maxInstructions;
};

int Process::getCurrInstructions() const{
    return currInstruction;
};

size_t Process::getMemRequired() const{
    return memoryRequired;
}

Process::ProcessState Process::getProcessState() const {
    return currentState;
}
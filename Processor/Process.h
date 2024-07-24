#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <iostream>
#include <fstream>
#include <random>
#include <unordered_map>


#include "../Command/ICommand.h"

class Process{
public:
    struct RequirementFlags
    {
        bool requireFiles;
        int numFiles;
        bool requireMemory;
        int memoryRequired;
    };
    enum ProcessState
    {
        READY,
        RUNNING,
        WAITING,
        FINISHED
    };

    Process(int pid, const std::string &name, RequirementFlags requirementFlags, const std::string &timestamp,
            int minInstructions, int maxInstructions, int minMem, int maxMem, int minPage, int maxPage);
    void addCommand(ICommand::CommandType commandType);
    void executeCurrentCommand() const;
    void moveToNextLine();

    int getPID() const;
    int getCommandCounter() const;
    std::string getName() const;
    std::string getTimestamp() const;
    int getCpuCoreID() const;
    int getMinInstructions() const;
    int getMaxInstructions() const;
    int getCurrInstructions() const;
    size_t getMemRequired() const;
    int getNumPage() const;
    ProcessState getProcessState() const;
    void* allocatedMemory = nullptr;

    std::vector<size_t> allocatedFrames;

    void executeTask();
    void executeTask(int quantumCycles);
    void generateRandomInstruction(int min, int max);
    void generateRandomMemReq(int minMem, int maxMem);
    void generateRandomPageReq(int minPage, int maxPage);

private:
    int pid;
    std::string name;
    typedef std::vector<std::shared_ptr<ICommand>> CommandList;
    CommandList commandList;
    size_t memoryRequired;
    std::mutex mutex;
    int commandCounter;
    int cpuCoreID = -1;
    RequirementFlags requirementFlags;
    ProcessState currentState;
    std::string timestamp;
    int minInstructions;
    int maxInstructions;
    int currInstruction;
    int numInstruction;
    int minMem;
    int maxMem;
    int numPage;
    int minPage;
    int maxPage;

    friend class ResourceEmulator;
    friend class Scheduler;
};

#endif
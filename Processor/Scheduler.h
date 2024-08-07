#pragma once
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cstdlib>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include <iomanip>
#include <condition_variable>

#include "Process.h"
#include "../Memory/Memory.h"
#include "../UI/UI_Manager.h"

class Scheduler{
public:
    static Scheduler& getInstance();
    static void initialize();
    static void destroy();
    void addProcess(std::shared_ptr<Process> process);
    void run();
    void shutdown();
    std::condition_variable processCV;

    int getNumCores() const;
    double getBatchProcessFrequency() const;
    int getMinInstructions() const;
    int getMaxInstructions() const;
    int getMinMem() const;
    int getMaxMem() const;
    int getMinPage() const;
    int getMaxPage() const;
    std::vector<std::shared_ptr<Process>> getRunningProcesses();
    std::vector<std::shared_ptr<Process>> getFinishedProcesses() const;

    void setMemory(Memory& mem);

    void getInfo();
    void readConfigFile(const std::string& filename);

    int numPagedIn;
    int numPagedOut;
    int activeCPUTicks;
    int idleCPUTicks;
    static int qqCounter;
    void generateQuantumCycleTxtFile(int quantumCycle);

private:
    Scheduler();
    ~Scheduler();
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    std::string schedulerAlgorithm;
    int quantumCycles;
    double batchProcessFrequency;
    int minPage;
    int maxPage;
    int minInstructions;
    int maxInstructions;
    double delaysPerExecution;

    void firstComeFirstServe();
    void roundRobin(int quantumCycles);

    std::queue<std::shared_ptr<Process>> readyQueue;
    std::vector<std::thread> coreThreads;
    std::vector<std::shared_ptr<Process>> runningProcesses;
    std::vector<std::shared_ptr<Process>> finishedProcesses;

    std::mutex processMutex;
    mutable std::mutex queueMutex;
    
    int numCores;
    bool running;
    bool createProcess;

    Process* selectRandomProcessToSwapOut();

    Memory* memory;
    static Scheduler* sharedInstance;
};

#endif
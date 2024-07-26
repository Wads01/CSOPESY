#include "Scheduler.h"
#include <iostream>
#include <chrono>
#include <thread>

Scheduler* Scheduler::sharedInstance = nullptr;
int Scheduler::qqCounter = 0;

Scheduler::Scheduler() : schedulerAlgorithm("NULL"), quantumCycles(0), batchProcessFrequency(0),
    minInstructions(0), maxInstructions(0), delaysPerExecution(0), numCores(0),
    running(true){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    activeCPUTicks = 0;
    idleCPUTicks = 0;
    numPagedIn = 0;
    numPagedOut = 0;
    runningProcesses.resize(numCores, nullptr);
}

Scheduler::~Scheduler(){
    shutdown();
}

void Scheduler::initialize(){
    if (!sharedInstance)
        sharedInstance = new Scheduler();
}

void Scheduler::destroy(){
    if (sharedInstance){
        delete sharedInstance;
        sharedInstance = nullptr;
    }
}

Scheduler& Scheduler::getInstance(){
    if (!sharedInstance)
        initialize();

    return *sharedInstance;
}

void Scheduler::getInfo(){
    std::cout << "Scheduler Configuration Information:" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Number of CPU Cores: " << numCores << std::endl;
    std::cout << "Scheduler Algorithm: " << schedulerAlgorithm << std::endl;
    std::cout << "Quantum Cycles: " << quantumCycles << std::endl;
    std::cout << "Batch Process Frequency: " << batchProcessFrequency << std::endl;
    std::cout << "Minimum Instructions per Process: " << minInstructions << std::endl;
    std::cout << "Maximum Instructions per Process: " << maxInstructions << std::endl;
    std::cout << "Delays per Execution: " << delaysPerExecution << std::endl;
    std::cout << "Minimum Page per Process: " << minPage << std::endl;
    std::cout << "Maximum Page per Process: " << maxPage << std::endl;
    std::cout << "--------------------------------" << std::endl;

}

void Scheduler::addProcess(std::shared_ptr<Process> process){
    std::unique_lock<std::mutex> lock(queueMutex);

    // Add the process to queue
    readyQueue.push(process);
    process->currentState = Process::WAITING;
    processCV.notify_one();
}

void Scheduler::run(){
    if (schedulerAlgorithm == "rr")
        roundRobin(quantumCycles);
    else if (schedulerAlgorithm == "fcfs")
        firstComeFirstServe();
    else{
        std::cout << "Invalid Scheduler. Program Shutting Down" << std::endl;
    }
}



void Scheduler::firstComeFirstServe(){
    while (running){
        std::unique_lock<std::mutex> lock(queueMutex);

        // Wait for processes or termination signal
        if (readyQueue.empty()){
            ++idleCPUTicks;
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // Assign processes to available cores
        for (int coreID = 0; coreID < numCores; ++coreID){
            if (!runningProcesses[coreID] && !readyQueue.empty()){
                auto process = readyQueue.front();
                readyQueue.pop();
                process->cpuCoreID = coreID;
                runningProcesses[coreID] = process;

                if (!Memory::getInstance().allocateMemory(process.get())){
                    process->currentState = Process::WAITING;
                    runningProcesses[coreID] = nullptr;
                    readyQueue.push(process);
                    continue;
                }

                // Start a thread for executing the process
                ++activeCPUTicks;
                coreThreads.emplace_back([this, process, coreID](){
                    process->currentState = Process::RUNNING;
                    process->executeTask();

                    {
                        std::lock_guard<std::mutex> lock(queueMutex);
                        if (process->currentState == Process::FINISHED){
                            finishedProcesses.push_back(process);
                            Memory::getInstance().deallocateMemory(process.get());
                        }
                        else{
                            process->currentState = Process::WAITING;
                            readyQueue.push(process);
                            Memory::getInstance().deallocateMemory(process.get());
                        }

                        runningProcesses[coreID] = nullptr;
                        processCV.notify_one();
                    }
                });
            }
        }

        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delaysPerExecution * 1000))); // Delays Per Execution
    }

    for (auto& thread : coreThreads)
        thread.join();
}

void Scheduler::roundRobin(int quantumCycles){
    while (running){
        std::unique_lock<std::mutex> lock(queueMutex);

        if (readyQueue.empty()){
            ++idleCPUTicks;
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // Assign processes to available cores
        for (int coreID = 0; coreID < numCores; ++coreID){
            if (!runningProcesses[coreID] && !readyQueue.empty()){
                auto process = readyQueue.front();
                readyQueue.pop();
                process->cpuCoreID = coreID;
                runningProcesses[coreID] = process;

                // Allocate memory and store the returned pointer in the process
                process->allocatedMemory = Memory::getInstance().allocateMemory(process.get());
                if (Memory::getInstance().getAllocator()->getName() == "PagingMemoryAllocator"){
                    if (!process->allocatedMemory){
                        auto pagingAllocator = dynamic_cast<PagingMemoryAllocator*>(memory->getAllocator());
                        Process* processToSwapOut = selectRandomProcessToSwapOut();

                        if (processToSwapOut){
                            pagingAllocator->writeProcessToBackingStore(process.get(), processToSwapOut);
                            pagingAllocator->readProcessFromBackingStore(process.get());
                        }
                        
                        process->currentState = Process::WAITING;
                        runningProcesses[coreID] = nullptr;
                        readyQueue.push(process);
                        continue;
                    }
                    numPagedIn += process->getNumPage();
                }
                else if (Memory::getInstance().getAllocator()->getName() == "FlatMemoryAllocator"){
                    if (!process->allocatedMemory){
                        // Simply push process back to queue
                        process->currentState = Process::WAITING;
                        runningProcesses[coreID] = nullptr;
                        readyQueue.push(process);
                        continue; 
                    }
                }

                // Start a thread for executing the process
                ++activeCPUTicks;
                coreThreads.emplace_back([this, process, coreID, quantumCycles](){
                    process->currentState = Process::RUNNING;
                    process->executeTask(quantumCycles);

                    generateQuantumCycleTxtFile(qqCounter);
                    ++qqCounter;

                    {
                        std::lock_guard<std::mutex> lock(queueMutex);

                        if (process->currentState == Process::FINISHED)
                        {
                            finishedProcesses.push_back(process);

                            Memory::getInstance().deallocateMemory(process.get());
                            numPagedOut += process->getNumPage();
                        }
                        else
                        {
                            process->currentState = Process::WAITING;
                            readyQueue.push(process);
                            
                            Memory::getInstance().deallocateMemory(process.get());
                            numPagedOut += process->getNumPage();
                        }

                        runningProcesses[coreID] = nullptr;
                        processCV.notify_one();
                    }
                });
            }
        }

        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delaysPerExecution * 1000))); //Delays Per Execution
    }

    for (auto& thread : coreThreads)
        thread.join();
}

void Scheduler::readConfigFile(const std::string& filename){
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening config file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)){
        std::istringstream iss(line);
        std::string key, value;
        if (iss >> key >> value){
            if (key == "num-cpu")
            {
                numCores = std::stoi(value);
                runningProcesses.resize(numCores, nullptr);
            }
            else if (key == "scheduler") { schedulerAlgorithm = value; }
            else if (key == "quantum-cycles") { quantumCycles = std::stoi(value); }
            else if (key == "batch-process-freq") { batchProcessFrequency = std::stod(value); }
            else if (key == "min-ins") { minInstructions = std::stoi(value); }
            else if (key == "max-ins") { maxInstructions = std::stoi(value); }
            else if (key == "delays-per-exec") { delaysPerExecution = std::stod(value); }
            else if (key == "min-page-per-proc") { minPage = std::stod(value); }
            else if (key == "max-page-per-proc") { maxPage = std::stod(value); }
        }
        else 
            std::cerr << "Warning: Unrecognized parameter in config file: " << key << std::endl;
    }

    file.close();
}

Process* Scheduler::selectRandomProcessToSwapOut(){
    std::vector<Process*> candidates;

    for (const auto& process : runningProcesses){
        if (process != nullptr && process->currentState == Process::RUNNING)
            candidates.push_back(process.get());
    }

    if (candidates.empty())
        return nullptr;

    size_t randomIndex = std::rand() % candidates.size();
    
    return candidates[randomIndex];
}

void Scheduler::generateQuantumCycleTxtFile(int quantumCycle){
    UI_Manager& ui = UI_Manager::getInstance();
    std::string timestamp = ui.generateTimestamp();

    int processesInMemory = 0;
    size_t totalExternalFragmentation = 0;
    size_t currentAddress = 0;
    std::vector<std::string> processEntries;

    std::vector<std::shared_ptr<Process>> runningProcesses_ = getRunningProcesses();

    for (const auto& process : runningProcesses_){
        if (process){
            ++processesInMemory;
            size_t upperLimit = currentAddress + process->getMemRequired();
            size_t lowerLimit = currentAddress;
            std::ostringstream entry;
            entry << upperLimit << "\n" << process->getName() << "\n" << lowerLimit << "\n\n" ;
            processEntries.push_back(entry.str());
            currentAddress = upperLimit;
        }
    }

    totalExternalFragmentation = Memory::getInstance().getMaxSize() - currentAddress;

    std::ostringstream fileContent;
    fileContent << "Timestamp: (" << timestamp << ")\n";
    fileContent << "Number of processes in memory: " << processesInMemory << "\n";
    fileContent << "Total external fragmentation in KB: " << totalExternalFragmentation << "\n\n";
    fileContent << "-------end------ = " << Memory::getInstance().getMaxSize() << "\n\n";

    for (auto it = processEntries.rbegin(); it != processEntries.rend(); ++it)
        fileContent << *it;

    fileContent << "--------start------- = 0\n";

    std::ostringstream filename;
    filename << "memory_stamp_" << std::setw(2) << std::setfill('0') << quantumCycle << ".txt";

    std::string directory = "z_memLogs_z";
    std::ofstream outFile(directory + "/" + filename.str());
    if (outFile.is_open()){
        outFile << fileContent.str();
        outFile.close();
    }
    else
        std::cerr << "Failed to create file: " << filename.str() << std::endl;
}

void Scheduler::setMemory(Memory& mem){
    memory = &mem;
}

int Scheduler::getNumCores() const{
    return numCores;
}

double Scheduler::getBatchProcessFrequency() const{
    return batchProcessFrequency;
}

int Scheduler::getMinInstructions() const{
    return minInstructions;
}

int Scheduler::getMaxInstructions() const{
    return maxInstructions;
}

int Scheduler::getMinMem() const {
    return Memory::getInstance().getMinMem();
}

int Scheduler::getMaxMem() const {
    return Memory::getInstance().getMaxMem();
}

int Scheduler::getMinPage() const {
    return minPage;
}

int Scheduler::getMaxPage() const {
    return maxPage;
}

std::vector<std::shared_ptr<Process>> Scheduler::getRunningProcesses() const{
    std::lock_guard<std::mutex> lock(queueMutex);
    std::vector<std::shared_ptr<Process>> running;

    for (const auto& process : runningProcesses){
        if (process != nullptr && process->currentState == Process::RUNNING){
            running.push_back(process);
        }
    }

    return running;
}

std::vector<std::shared_ptr<Process>> Scheduler::getFinishedProcesses() const{
    return finishedProcesses;
}

void Scheduler::shutdown(){
    running = false;
    processCV.notify_all(); // Notify all threads to wake up
    for (auto& thread : coreThreads){
        if (thread.joinable())
            thread.join();
    }
}

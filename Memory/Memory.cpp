#include <iostream>

#include "Memory.h"

Memory* Memory::sharedInstance = nullptr;

Memory& Memory::getInstance(){
    static Memory instance;
    return instance;
}

Memory::Memory() : currentOverallMemoryUsage(0), allocator(nullptr) {}

Memory::~Memory(){
    destroy();
}

void Memory::initialize(){
    if (!sharedInstance)
        sharedInstance = new Memory();
}

void Memory::destroy(){
    if (sharedInstance){
        delete sharedInstance;
        sharedInstance = nullptr;
    }
}

void Memory::readConfigFile(const std::string& filename){
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Error opening config file: " + filename);

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (iss >> key >> value) {
            if (key == "max-overall-mem")
                maxOverallMemory = std::stoi(value);
            else if (key == "min-mem-per-proc")
                minMemoryPerProcess = std::stoi(value);
            else if (key == "max-mem-per-proc")
                maxMemoryPerProcess = std::stoi(value);
            else if (key == "min-page-per-proc")
                minPagePerProcess = std::stoi(value);
            else if (key == "max-page-per-proc")
                maxPagePerProcess = std::stoi(value);
        }
        else
            std::cerr << "Warning: Unrecognized parameter in config file: " << key << std::endl;
    }

    if(minPagePerProcess == 1 && maxPagePerProcess == 1)
        allocator = std::make_unique<FlatMemoryAllocator>(maxOverallMemory);
    else{
        auto pagingAllocator = std::make_unique<PagingMemoryAllocator>(maxOverallMemory);
        pagingAllocator->setMinPageCount(minPagePerProcess);
        pagingAllocator->setMaxPageCount(maxPagePerProcess);
        allocator = std::move(pagingAllocator);
    }

    // Create Backing Store text File
    createBackingStore();

    file.close();
}

void* Memory::allocateMemory(Process* process){
    void* ptr = allocator->allocate(process);
    if(ptr)
        currentOverallMemoryUsage += process->getMemRequired();

    return ptr;
}

void Memory::deallocateMemory(Process* process){
    if (process){
        size_t deallocatedMem = allocator->deallocate(process);
        if (deallocatedMem > 0)
            currentOverallMemoryUsage -= deallocatedMem;
        else
            std::cerr << "Failed to deallocate memory at pointer: " << process->getPID() << std::endl;
    }
}

size_t Memory::getCurrentOverallMemoryUsage() const{
    return currentOverallMemoryUsage;
}

std::string Memory::visualizeMemory() const{
    return allocator->visualizeMemory();
}

void Memory::getInfo(){
    std::cout << "Memory Configuration Information:" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Max Overall Memory: " << maxOverallMemory << " KB" << std::endl;
    std::cout << "Min Memory Per Process: " << minMemoryPerProcess << " KB" << std::endl;
    std::cout << "Max Memory Per Process: " << maxMemoryPerProcess << " KB" << std::endl;
    std::cout << "Min Page Per Process: " << minPagePerProcess << " KB" << std::endl;
    std::cout << "Max Page Per Process: " << maxPagePerProcess << " KB" << std::endl;
    std::cout << "Memory Allocator: " << allocator->getName() << std::endl;
}

void Memory::createBackingStore(){
    std::string backingStoreFile = "backingstore.txt";

    std::ifstream infile(backingStoreFile);
    if (!infile.good()){
        std::ofstream outfile(backingStoreFile);
        if (!outfile)
            throw std::runtime_error("Error creating backing store file: " + backingStoreFile);
    }

    infile.close();
}

int Memory::getMinMem() const{
    return minMemoryPerProcess;
}

int Memory::getMaxMem() const{
    return maxMemoryPerProcess;
}

size_t Memory::getMaxSize() const{
    return allocator->getMaxSize();
}


IMemoryAllocator* Memory::getAllocator(){
    return allocator.get();
}
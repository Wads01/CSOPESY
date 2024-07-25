#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <memory>

#include "IMemoryAllocator.h"
#include "FlatMemoryAllocator.h"
#include "PagingMemoryAllocator.h"

class Memory{
public:
    static Memory& getInstance();
    static void initialize();
    static void destroy();

    void getInfo();
    void readConfigFile(const std::string& filename);
    void* allocateMemory(Process* process);
    void deallocateMemory(Process* process);

    size_t getCurrentOverallMemoryUsage() const;
    std::string visualizeMemory() const;

    int getMinMem() const;
    int getMaxMem() const;
    size_t getMaxSize() const;
    IMemoryAllocator* getAllocator();

private:
    Memory();
    ~Memory();
    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;

    int maxOverallMemory;
    int minMemoryPerProcess;
    int maxMemoryPerProcess;
    int minPagePerProcess;
    int maxPagePerProcess;

    size_t currentOverallMemoryUsage;

    void createBackingStore();

    std::unique_ptr<IMemoryAllocator> allocator;
    static Memory* sharedInstance;
};

#endif

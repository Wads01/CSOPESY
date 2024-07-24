#pragma once
#ifndef PAGINGMEMORYALLOCATOR_H
#define PAGINGMEMORYALLOCATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <sstream>
#include <algorithm>

#include "IMemoryAllocator.h"
#include "../Processor/Process.h"

class PagingMemoryAllocator : public IMemoryAllocator {
public:
    PagingMemoryAllocator(size_t maxSize);
    ~PagingMemoryAllocator();

    void* allocate(Process* process) override;
    size_t deallocate(Process* process) override;
    std::string visualizeMemory() override;

    size_t getMaxSize() const override;
    std::string getName() const override;

    void setMinPageCount(int minPageCount);
    void setMaxPageCount(int maxPageCount);

    bool loadPages(Process* process);
private:
    int minPageCount;
    int maxPageCount;

    size_t maxSize;
    size_t numFrames;
    std::unordered_map<size_t, size_t> frameMap;
    std::vector<size_t> freeFrameList;
    
    size_t allocateFrames(size_t numFrames, size_t processID);
    void deallocateFrames(size_t numFrames, size_t frameIndex);
};

#endif
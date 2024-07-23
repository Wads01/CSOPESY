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

    void* allocate(int processID, size_t size) override;
    size_t deallocate(void* ptr) override;
    std::string visualizeMemory() override;

    size_t getMaxSize() const override;
    std::string getName() const override;

private:
    size_t maxSize;
    size_t numFrames;
    size_t allocatedSize;
    std::unordered_map<size_t, size_t> frameMap;
    std::vector<size_t> freeFrameList;
    std::unordered_map<int, std::vector<size_t>> processFrameMap;

    size_t allocateFrames(size_t numFrames, int processID);
    void deallocateFrames(size_t numFrames, size_t frameIndex);
    bool loadPages(int processID, size_t numPages);
};

#endif
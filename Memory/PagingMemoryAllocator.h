#pragma once
#ifndef PAGINGMEMORYALLOCATOR_H
#define PAGINGMEMORYALLOCATOR_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <optional>
#include <mutex>

#include "IMemoryAllocator.h"
#include "../Processor/Process.h"

class PagingMemoryAllocator : public IMemoryAllocator {
public:
    PagingMemoryAllocator(size_t maxSize);
    ~PagingMemoryAllocator();

    void* allocate(Process* process) override;
    size_t deallocate(Process* process) override;
    std::string visualizeMemory() override;

    void writePageToBackingStore(int processID, int pageNumber, const std::vector<char>& pageData);
    std::optional<std::vector<char>> loadPageFromBackingStore(int processID, int pageNumber, size_t pageSize);

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
    
    std::mutex allocationMutex;
    std::mutex backingStoreMutex;
    size_t allocateFrames(size_t numFrames, size_t processID);
    void deallocateFrames(size_t numFrames, size_t frameIndex);
    std::unordered_map<size_t, std::vector<char>> frameDataMap;
};

#endif
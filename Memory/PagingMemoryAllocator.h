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

    size_t getMaxSize() const override;
    std::string getName() const override;
    size_t getTotalMemReqProc() const;
    size_t getActiveMem() const;


    void writePageToBackingStore(int processID, int pageNumber, const std::vector<char>& pageData);
    std::optional<std::vector<char>> loadPageFromBackingStore(int processID, int pageNumber, size_t pageSize);
    
private:
    std::mutex allocationMutex;
    std::mutex backingStoreMutex;

    size_t maxSize;
    size_t activeMem;
    size_t totalMemReqProc;
    std::unordered_map<size_t, size_t> activeMemMap;
    std::unordered_map<size_t, size_t> frameMap;
    std::vector<size_t> freeFrameList;

    size_t setPageSize(size_t memPerPage);
    std::vector<size_t> allocateFrames(size_t numPages, size_t memPerPage, size_t processID);
};

#endif
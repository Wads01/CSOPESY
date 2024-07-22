#pragma once
#ifndef PAGINGMEMORYALLOCATOR_H
#define PAGINGMEMORYALLOCATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <sstream>

#include "IMemoryAllocator.h"

class PagingMemoryAllocator : public IMemoryAllocator {
public:
    PagingMemoryAllocator(size_t maxSize);
    ~PagingMemoryAllocator();

    void* allocate(size_t size) override;
    size_t deallocate(void* ptr) override;
    std::string visualizeMemory() override;

    size_t getMaxSize() const override;
    std::string getName() const override;

private:
    size_t maxSize;
    size_t allocatedSize;
    char* memory;
    std::unordered_map<void*, size_t> allocationMap;
    std::set<size_t> freeFrameList;

    size_t allocateFrames(size_t numFrames);
    void deallocateFrames(size_t frameIndex, size_t numFrames);
};

#endif
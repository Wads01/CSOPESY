#pragma once

#include <vector>
#include <map>
#include <string>
#include <cstddef>
#include <iostream>
#include <mutex>

#include "IMemoryAllocator.h"
#include "../Processor/Process.h"

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    FlatMemoryAllocator(size_t maxSize);
    ~FlatMemoryAllocator();

    void* allocate(Process* process) override;
    size_t deallocate(Process* process) override;
    
    size_t getMaxSize() const override;
    size_t getAllocatedSize() const;
    std::string getName() const;

private:
    size_t allocatedAmount;
    size_t maxSize;
    size_t allocatedSize;
    char* memory;
    std::vector<bool> allocationMap;
    std::map<void*, size_t> allocationSizes;

    bool canAlloc(size_t index, size_t size) const;
    void allocAt(size_t index, size_t size);
    size_t deallocAt(size_t index);

    std::mutex allocationMutex;
};
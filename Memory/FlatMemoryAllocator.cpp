#include <new>
#include <algorithm>
#include <cstring>

#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maxSize) : maxSize(maxSize), allocatedSize(0), allocationMap(maxSize, false){
    memory = new char[maxSize];
    std::memset(memory, '.', maxSize); // Initialize memory with '.'
}

FlatMemoryAllocator::~FlatMemoryAllocator(){
    delete[] memory;
}

void* FlatMemoryAllocator::allocate(Process* process){
    std::lock_guard<std::mutex> lock(allocationMutex);

    size_t memReq = process->getMemRequired();
    for (size_t i = 0; i <= maxSize - memReq; ++i) {
        if (canAlloc(i, memReq)) {
            allocAt(i, memReq);
            void* allocatedPtr = static_cast<void*>(&memory[i]);

            if (allocatedPtr < static_cast<void*>(memory) || allocatedPtr >= static_cast<void*>(memory + maxSize)){
                std::cerr << "Invalid allocated pointer for process: " << process->getPID() << std::endl;
                return nullptr;
            }

            return allocatedPtr;
        }
    }
    return nullptr;
}

size_t FlatMemoryAllocator::deallocate(Process* process){
    std::lock_guard<std::mutex> lock(allocationMutex);
    
    void* allocatedMemory = process->allocatedMemory;

    if (allocatedMemory < static_cast<void*>(memory) || allocatedMemory >= static_cast<void*>(memory + maxSize)) {
        std::cerr << "Pointer out of bounds for process: " << process->getPID() << std::endl;
        return 0;
    }

    auto it = allocationSizes.find(allocatedMemory);
    if (it == allocationSizes.end()) {
        std::cerr << "Invalid deallocation attempt for process: " << process->getPID() << std::endl;
        return 0;
    }

    size_t index = static_cast<char*>(allocatedMemory) - memory;
    return deallocAt(index);
}

bool FlatMemoryAllocator::canAlloc(size_t index, size_t size) const{
    for (size_t i = 0; i < size; ++i){
        if (allocationMap[index + i])
            return false;
    }
    return true;
}

void FlatMemoryAllocator::allocAt(size_t index, size_t size){
    for (size_t i = 0; i < size; ++i) {
        allocationMap[index + i] = true;
        new (&memory[index + i]) char('X');
    }
    allocationSizes[&memory[index]] = size;
    allocatedSize += size;
}

size_t FlatMemoryAllocator::deallocAt(size_t index){
    auto it = allocationSizes.find(&memory[index]);
    if (it == allocationSizes.end()) {
        std::cerr << "Invalid deallocation attempt at index: " << index << std::endl;
        return 0;
    }

    size_t size = it->second;
    for (size_t i = 0; i < size; ++i){
        allocationMap[index + i] = false;
        new (&memory[index + i]) char('.');
    }
    allocationSizes.erase(it);
    allocatedSize -= size;

    return size;
}

size_t FlatMemoryAllocator::getMaxSize() const{
    return maxSize;
}

size_t FlatMemoryAllocator::getAllocatedSize() const{
    return allocatedSize;
}

std::string FlatMemoryAllocator::getName() const{
    return "FlatMemoryAllocator";
}
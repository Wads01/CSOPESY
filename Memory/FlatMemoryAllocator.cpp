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
    size_t memReq = process->getMemRequired();
    for (size_t i = 0; i <= maxSize - memReq; ++i){
        if (canAlloc(i, memReq)){
            allocAt(i, memReq);
            return static_cast<void*>(&memory[i]);
        }
    }
    return nullptr;
}

size_t FlatMemoryAllocator::deallocate(void* ptr){
    if (ptr < static_cast<void*>(memory) || ptr >= static_cast<void*>(memory + maxSize)){
        std::cerr << "Pointer out of bounds: " << ptr << std::endl;
        return 0;
    }

    auto it = allocationSizes.find(ptr);
    if (it == allocationSizes.end()){
        std::cerr << "Invalid deallocation attempt at pointer: " << ptr << std::endl;
        return 0;
    }

    size_t index = static_cast<char*>(ptr) - memory;
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

std::string FlatMemoryAllocator::visualizeMemory(){
    return std::string(memory, memory + maxSize);
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
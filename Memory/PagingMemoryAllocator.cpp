#include "PagingMemoryAllocator.h"
#include <new>
#include <cstring>

PagingMemoryAllocator::PagingMemoryAllocator(size_t maxSize) : maxSize(maxSize), allocatedSize(0){
    memory = new char[maxSize * sizeof(size_t)];
    for (size_t i = 0; i < maxSize; ++i)
        freeFrameList.insert(i);
}

PagingMemoryAllocator::~PagingMemoryAllocator(){
    delete[] memory;
}

void* PagingMemoryAllocator::allocate(size_t size){
    if (freeFrameList.size() < size)
        return nullptr;

    size_t firstFrameIndex = allocateFrames(size);
    void* allocatedPtr = static_cast<void*>(memory + firstFrameIndex * sizeof(size_t));
    
    for (size_t i = 0; i < size; ++i)
        new (memory + (firstFrameIndex + i) * sizeof(size_t)) size_t(0);

    allocationMap[allocatedPtr] = size;
    allocatedSize += size;

    return allocatedPtr;
}

size_t PagingMemoryAllocator::deallocate(void* ptr){

    if (ptr < static_cast<void*>(memory) || ptr >= static_cast<void*>(memory + maxSize * sizeof(size_t))){
        std::cerr << "Pointer out of bounds: " << ptr << std::endl;
        return 0;
    }

    auto it = allocationMap.find(ptr);
    if (it == allocationMap.end()){
        std::cerr << "Invalid deallocation attempt at pointer: " << ptr << std::endl;
        return 0;
    }

    size_t size = it->second;
    size_t frameIndex = (static_cast<char*>(ptr) - memory) / sizeof(size_t);

    deallocateFrames(frameIndex, size);

    allocationMap.erase(it);
    allocatedSize -= size;

    return size;
}

std::string PagingMemoryAllocator::visualizeMemory(){
    std::ostringstream oss;
    oss << "Paging Memory Visualization\n";
    oss << "Max Size: " << maxSize << " KB\n";
    oss << "Allocated Size: " << allocatedSize << " KB\n";
    oss << "Free Frames: " << freeFrameList.size() << "/" << maxSize << "\n";
    oss << "Allocated Frames: " << (maxSize - freeFrameList.size()) << "/" << maxSize << "\n";
    return oss.str();
}

size_t PagingMemoryAllocator::getMaxSize() const{
    return maxSize;
}

size_t PagingMemoryAllocator::allocateFrames(size_t numFrames){
    auto it = freeFrameList.begin();
    size_t firstFrameIndex = *it;
    for (size_t i = 0; i < numFrames; ++i)
        freeFrameList.erase(it++);

    return firstFrameIndex;
}

void PagingMemoryAllocator::deallocateFrames(size_t frameIndex, size_t numFrames){
    for (size_t i = 0; i < numFrames; ++i)
        freeFrameList.insert(frameIndex + i);
}

std::string PagingMemoryAllocator::getName() const{
    return "PagingMemoryAllocator";
}
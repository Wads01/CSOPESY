#include <new>
#include <cstring>

#include "PagingMemoryAllocator.h"


PagingMemoryAllocator::PagingMemoryAllocator(size_t maxSize) : maxSize(maxSize), numFrames(maxSize){
    for (size_t i = 0; i < numFrames; ++i)
        freeFrameList.push_back(i);
}

PagingMemoryAllocator::~PagingMemoryAllocator() {}

void* PagingMemoryAllocator::allocate(int processID, size_t size){
    size_t numFramesRequired = (size + sizeof(size_t) - 1) / sizeof(size_t);

    if (freeFrameList.size() < numFramesRequired)
        return nullptr; // Not enough frames available

    if (!loadPages(processID, numFramesRequired))
        return nullptr; // No Frames Allocated

    size_t firstFrameIndex = allocateFrames(numFramesRequired, processID);

    if(firstFrameIndex == 0)
        return nullptr;

    void* allocatedPtr = reinterpret_cast<void*>(firstFrameIndex * sizeof(size_t));
    frameMap[firstFrameIndex] = numFramesRequired;

    return allocatedPtr;
}

size_t PagingMemoryAllocator::deallocate(void* ptr){
    size_t frameIndex = reinterpret_cast<size_t>(ptr) / sizeof(size_t);
    auto it = frameMap.find(frameIndex);
    if (it == frameMap.end()) {
        std::cerr << "Invalid deallocation attempt at pointer: " << ptr << std::endl;
        return 0;
    }

    size_t numFrames = it->second;
    deallocateFrames(numFrames, frameIndex);
    frameMap.erase(it);
    return numFrames * sizeof(size_t);
}

std::string PagingMemoryAllocator::visualizeMemory(){
    std::ostringstream oss;
    oss << "Paging Memory Visualization\n";
    oss << "Max Size: " << maxSize << " KB\n";
    oss << "Free Frames: " << freeFrameList.size() << "/" << numFrames << "\n";
    oss << "Allocated Frames: " << (numFrames - freeFrameList.size()) << "/" << numFrames << "\n";
    return oss.str();
}

size_t PagingMemoryAllocator::allocateFrames(size_t numFrames, int processID){
    if (freeFrameList.size() < numFrames)
        return 0;

    std::vector<size_t> allocatedFrames;
    size_t firstFrameIndex = freeFrameList.back();
    for (size_t i = 0; i < numFrames; ++i){
        allocatedFrames.push_back(freeFrameList.back());
        freeFrameList.pop_back();
    }

    processFrameMap[processID] = allocatedFrames;
    return firstFrameIndex;
}

void PagingMemoryAllocator::deallocateFrames(size_t numFrames, size_t frameIndex){
    for (size_t i = 0; i < numFrames; ++i)
        freeFrameList.push_back(frameIndex + i);
}

bool PagingMemoryAllocator::loadPages(int processID, size_t numPages){
    if (freeFrameList.size() < numPages)
        return false; // Not enough frames available

    size_t firstFrameIndex = allocateFrames(numPages, processID);

    if (firstFrameIndex == 0)
        return false; // No Frames Allocated | Failed

    return true;
}

size_t PagingMemoryAllocator::getMaxSize() const{
    return maxSize;
}

std::string PagingMemoryAllocator::getName() const{
    return "PagingMemoryAllocator";
}
#include <new>
#include <cstring>

#include "PagingMemoryAllocator.h"


PagingMemoryAllocator::PagingMemoryAllocator(size_t maxSize) : maxSize(maxSize), numFrames(maxSize), allocatedSize(0) {
    // Initialize free frame list
    for (size_t i = 0; i < numFrames; ++i)
        freeFrameList.push_back(i);
}

PagingMemoryAllocator::~PagingMemoryAllocator() {
    // Destructor
}

void* PagingMemoryAllocator::allocate(int processID, size_t size) {
    // Calculate number of frames required
    size_t numPages = (size + sizeof(size_t) - 1) / sizeof(size_t); // Page size in frames
    
    // Check if enough free frames are available
    if (freeFrameList.size() < numPages)
        return nullptr;

    // Allocate frames
    size_t frameIndex = allocateFrames(numPages, processID);
    if (frameIndex == -1)
        return nullptr;

    // Return pointer to allocated memory
    return reinterpret_cast<void*>(frameIndex * sizeof(size_t));
}

size_t PagingMemoryAllocator::deallocate(void* ptr) {
    size_t frameIndex = reinterpret_cast<size_t>(ptr) / sizeof(size_t);

    // Check if pointer is valid
    if (frameIndex >= numFrames || frameMap.find(frameIndex) == frameMap.end()) {
        std::cerr << "Invalid deallocation attempt at pointer: " << ptr << std::endl;
        return 0;
    }

    size_t processID = frameMap[frameIndex];
    size_t numFramesToDeallocate = 0;

    // Find how many frames to deallocate for this process
    for (auto it = frameMap.begin(); it != frameMap.end(); ) {
        if (it->second == processID) {
            numFramesToDeallocate++;
            deallocateFrames(1, it->first);
            it = frameMap.erase(it); // Remove the frame from the map
        } else {
            ++it;
        }
    }

    allocatedSize -= numFramesToDeallocate;
    return numFramesToDeallocate * sizeof(size_t);
}

std::string PagingMemoryAllocator::visualizeMemory() {
    std::ostringstream oss;
    oss << "Paging Memory Visualization\n";
    oss << "Max Size: " << maxSize << " KB\n";
    oss << "Allocated Size: " << allocatedSize * sizeof(size_t) / 1024 << " KB\n";
    oss << "Free Frames: " << freeFrameList.size() << "/" << numFrames << "\n";
    oss << "Allocated Frames: " << numFrames - freeFrameList.size() << "/" << numFrames << "\n";
    return oss.str();
}

size_t PagingMemoryAllocator::allocateFrames(size_t numFrames, size_t processID) {
    if (numFrames > freeFrameList.size())
        return -1;

    size_t startFrameIndex = freeFrameList.back() - numFrames + 1;
    for (size_t i = 0; i < numFrames; ++i) {
        frameMap[startFrameIndex + i] = processID;
    }
    
    freeFrameList.erase(freeFrameList.end() - numFrames, freeFrameList.end());
    allocatedSize += numFrames;

    return startFrameIndex;
}

void PagingMemoryAllocator::deallocateFrames(size_t numFrames, size_t frameIndex) {
    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(frameIndex + i);
        frameMap.erase(frameIndex + i);
    }

    std::sort(freeFrameList.begin(), freeFrameList.end());
}

bool PagingMemoryAllocator::loadPages(Process* process, size_t numPages) {
    // This function might need additional implementation based on how you want to handle pages
    // For now, it returns true if enough frames are available
    return (freeFrameList.size() >= numPages);
}

std::string PagingMemoryAllocator::getName() const {
    return "PagingMemoryAllocator";
}
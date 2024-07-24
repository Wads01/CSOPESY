#include <new>
#include <cstring>

#include "PagingMemoryAllocator.h"


PagingMemoryAllocator::PagingMemoryAllocator(size_t maxSize) : maxSize(maxSize), numFrames(maxSize){
    for(size_t i = 0; i < numFrames; i++)
        freeFrameList.push_back(i);
}

PagingMemoryAllocator::~PagingMemoryAllocator(){}

void* PagingMemoryAllocator::allocate(Process* process){
    size_t processID = process->getPID();
    size_t numFramesNeeded = process->getNumPage();

    if(numFramesNeeded > freeFrameList.size()){
        std::cerr << "Allocation Failed. Not Enough Free Frames." << std::endl;
        return nullptr;
    }

    size_t frameIndex = allocateFrames(numFramesNeeded, processID);

    return reinterpret_cast<void*>(frameIndex);
}

size_t PagingMemoryAllocator::deallocate(Process* process){
    size_t processID = process->getPID();

    auto it = std::find_if(frameMap.begin(), frameMap.end(), [processID](const auto& entry){
        return entry.second == processID;
    });


    while (it != frameMap.end()) {
        size_t frameIndex = it->first;
        deallocateFrames(1, frameIndex);
        it = std::find_if(frameMap.begin(), frameMap.end(), [processID](const auto& entry){
            return entry.second == processID;
        });
    }

    return process->getMemRequired();
}

std::string PagingMemoryAllocator::visualizeMemory(){
    std::ostringstream oss;
    oss << "Paging Memory Visualization\n";
    oss << "Max Size: " << maxSize << " KB\n";
    oss << "Allocated Size: " << (numFrames - freeFrameList.size()) << " KB\n";
    oss << "Free Frames: " << freeFrameList.size() << "/" << numFrames << "\n";
    oss << "Allocated Frames: " << (numFrames - freeFrameList.size()) << "/" << numFrames << "\n";

    return oss.str();
}

size_t PagingMemoryAllocator::allocateFrames(size_t numFrames, size_t processID){
    std::vector<size_t> allocatedFrames;

    for (size_t i = 0; i < numFrames; ++i) {
        size_t frameIndex = freeFrameList.back();
        freeFrameList.pop_back();
        frameMap[frameIndex] = processID;
        allocatedFrames.push_back(frameIndex);
    }

    return allocatedFrames.front();

}

void PagingMemoryAllocator::deallocateFrames(size_t numFrames, size_t frameIndex){
    for(size_t i = 0; i < numFrames; ++i)
        frameMap.erase(frameIndex + i);

    for(size_t i = 0; i < numFrames; ++i)
        freeFrameList.push_back(frameIndex + i);
}

bool PagingMemoryAllocator::loadPages(Process* process){
    size_t processID = process->getPID();
    size_t numPages = process->getNumPage();

    if (numPages > freeFrameList.size()){
        std::cerr << "Loading Pages Failed. Not Enough Free Frames." << std::endl;
        return false;
    }

    for (size_t i = 0; i < numPages; ++i) {
        size_t frameIndex = freeFrameList.back();
        freeFrameList.pop_back();
        frameMap[frameIndex] = processID;
    }

    return true;
}

size_t PagingMemoryAllocator::getMaxSize() const{
    return maxSize;
}

std::string PagingMemoryAllocator::getName() const{
    return "PagingMemoryAllocator";
}

void PagingMemoryAllocator::setMinPageCount(int minPageCount){
    this->minPageCount = minPageCount;
}

void PagingMemoryAllocator::setMaxPageCount(int maxPageCount){
    this->maxPageCount = maxPageCount;
}
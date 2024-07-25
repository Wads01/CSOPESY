#include <new>
#include <cmath>
#include <cstring>

#include "PagingMemoryAllocator.h"

PagingMemoryAllocator::PagingMemoryAllocator(size_t maxSize) : maxSize(maxSize){
    for (size_t i = 0; i < maxSize; ++i) 
        freeFrameList.push_back(i);
}

PagingMemoryAllocator::~PagingMemoryAllocator() {}

void* PagingMemoryAllocator::allocate(Process* process) {
    std::lock_guard<std::mutex> lock(allocationMutex);

    size_t processID = process->getPID();
    size_t numPages = process->getNumPage();
    size_t memPerPage = process->getMemPerPage();

    // Determine Page Size Depending on memPerPage
    size_t pageSize = setPageSize(memPerPage);

    totalMemReqProc = numPages * pageSize;

    if (totalMemReqProc > freeFrameList.size())
        return nullptr;

    std::vector<size_t> frameIndices = allocateFrames(numPages, pageSize, processID);

    if (frameIndices.empty()){
        std::cerr << "Allocation Failed. Could not allocate frames." << std::endl;
        return nullptr;
    }
    else{
        // Active Memory
        size_t activeMem = numPages * memPerPage;
        activeMemMap.insert({processID, activeMem});

        return reinterpret_cast<void*>(frameIndices.front());
    }
}

std::vector<size_t> PagingMemoryAllocator::allocateFrames(size_t numPages, size_t pageSize, size_t processID) {
    std::vector<size_t> allocatedFrames;

    for (size_t i = 0; i < numPages; ++i){
        for (size_t j = 0; j < pageSize; ++j){
            if (freeFrameList.empty()){
                std::cerr << "Error: Not enough free frames available." << std::endl;
                return {};
            }

            size_t frameIndex = freeFrameList.back();
            freeFrameList.pop_back();
            frameMap[frameIndex] = processID;

            allocatedFrames.push_back(frameIndex);
        }
    }

    return allocatedFrames;
}

size_t PagingMemoryAllocator::deallocate(Process* process){
    std::lock_guard<std::mutex> lock(allocationMutex);

    size_t processID = process->getPID();
    std::vector<size_t> deallocatedFrames;
    auto it = frameMap.begin();

    while (it != frameMap.end()){
        if (it->second == processID){
            size_t frameIndex = it->first;
            freeFrameList.push_back(frameIndex);
            deallocatedFrames.push_back(frameIndex);
            it = frameMap.erase(it);
        }
        else
            ++it;
    }

    activeMemMap.erase(processID);

    return deallocatedFrames.size();
}

size_t PagingMemoryAllocator::setPageSize(size_t memPerPage){
    // Find the smallest power of 2 greater than or equal to memPerPage
    size_t powerOfTwo = 1;
    while (powerOfTwo < memPerPage)
        powerOfTwo *= 2;

    return powerOfTwo;
}

size_t PagingMemoryAllocator::getMaxSize() const{
    return maxSize;
}

std::string PagingMemoryAllocator::getName() const{
    return "PagingMemoryAllocator";
}

size_t PagingMemoryAllocator::getTotalMemReqProc() const{
return totalMemReqProc;
}

size_t PagingMemoryAllocator::getActiveMem() const{
    size_t totalActiveMemory = 0;

    for (const auto& pair : activeMemMap)
        totalActiveMemory += pair.second;

    return totalActiveMemory;
}
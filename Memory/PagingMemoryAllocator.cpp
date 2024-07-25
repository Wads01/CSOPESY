#include <new>
#include <cstring>

#include "PagingMemoryAllocator.h"


PagingMemoryAllocator::PagingMemoryAllocator(size_t maxSize) : maxSize(maxSize), numFrames(maxSize){
    for(size_t i = 0; i < numFrames; i++)
        freeFrameList.push_back(i);
}

PagingMemoryAllocator::~PagingMemoryAllocator(){}

void* PagingMemoryAllocator::allocate(Process* process){
    std::lock_guard<std::mutex> lock(allocationMutex);

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
    std::lock_guard<std::mutex> lock(allocationMutex);


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

void PagingMemoryAllocator::writePageToBackingStore(int processID, int pageNumber, const std::vector<char>& pageData){
    std::ofstream outfile("backingstore.txt", std::ios::app | std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Error opening backing store for writing" << std::endl;
        return;
    }

    outfile.write(reinterpret_cast<const char*>(&processID), sizeof(processID));
    outfile.write(reinterpret_cast<const char*>(&pageNumber), sizeof(pageNumber));
    outfile.write(pageData.data(), pageData.size());

    outfile.close();
}

std::optional<std::vector<char>> readPageFromBackingStore(int processID, int pageNumber, size_t pageSize){
    std::ifstream infile("backingstore.txt", std::ios::in | std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Error opening backing store for reading" << std::endl;
        return std::nullopt;
    }

    while (infile) {
        int storedProcessID;
        int storedPageNumber;
        infile.read(reinterpret_cast<char*>(&storedProcessID), sizeof(storedProcessID));
        infile.read(reinterpret_cast<char*>(&storedPageNumber), sizeof(storedPageNumber));

        if (infile && storedProcessID == processID && storedPageNumber == pageNumber) {
            std::vector<char> pageData(pageSize);
            infile.read(pageData.data(), pageSize);
            infile.close();
            return pageData;
        } else {
            infile.ignore(pageSize);
        }
    }

    infile.close();
    return std::nullopt;
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
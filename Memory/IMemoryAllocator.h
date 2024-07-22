#pragma once
#ifndef IMEMORYALLOCATOR_H
#define IMEMORYALLOCATOR_H

#include <iostream>
#include <string>

class IMemoryAllocator{
public:
    virtual void *allocate(size_t size) = 0;
    virtual size_t deallocate(void *ptr) = 0;
    virtual std::string visualizeMemory() = 0;
    virtual size_t getMaxSize() const = 0;
    virtual std::string getName() const = 0;
};

#endif
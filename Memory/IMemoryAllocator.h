#pragma once
#ifndef IMEMORYALLOCATOR_H
#define IMEMORYALLOCATOR_H

#include <iostream>
#include <string>

#include "../Processor/Process.h"


class IMemoryAllocator{
public:
    virtual void *allocate(Process* process) = 0;
    virtual size_t deallocate(Process* process) = 0;
    virtual size_t getMaxSize() const = 0;
    virtual std::string getName() const = 0;
};

#endif
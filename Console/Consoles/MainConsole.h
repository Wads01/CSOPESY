#pragma once
#ifndef MAIN_CONSOLE_H
#define MAIN_CONSOLE_H

#include <atomic>

#include "../BaseScreen.h"
#include "../../Processor/Process.h"
#include "../../Processor/Scheduler.h"
#include "../../Memory/Memory.h"

class MainConsole : public BaseScreen{
public:
    MainConsole();
    ~MainConsole();
    void onEnabled() override;
    void display() override;
    void process() override;
    void processCommand(const std::string &command);
    
    void initialization(const std::string &command);
    void printDetails();
    void schedulerStart();
    void schedulerStop();

    int secondsToMilliseconds(double seconds);

    std::thread schedulerStartThread;

private:
    std::atomic<bool> runScheduler { false };
    bool initialized = false;
    Scheduler* scheduler;
    Memory* memory;
};

#endif
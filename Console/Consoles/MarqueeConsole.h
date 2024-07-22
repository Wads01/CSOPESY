#pragma once
#ifndef MARQUEE_CONSOLE_H
#define MARQUEE_CONSOLE_H

#include "../BaseScreen.h"
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <deque>
#include <vector>
#include <thread>
#include <string>

class MarqueeConsole : public BaseScreen{
public:
    MarqueeConsole();
    ~MarqueeConsole();
    void onEnabled() override;
    void display() override;
    void process() override;
    void processMarqueeCommand(const std::string &command);
    void marquee(const std::string &text, std::atomic<bool>& running);
    void pollInput(std::atomic<bool>& running);
    void displayPreviousCommands(const std::deque<std::string>& previousCommands);
    
    std::mutex& getMutex();
    std::condition_variable& getCV();

private:
    void reset();

    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::thread marqueeThread;
    std::thread inputThread;
    std::vector<char> stackInput;
    std::deque<std::string> previousCommands;
    bool active;
};

#endif

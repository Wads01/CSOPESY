#pragma once
#ifndef PROCESS_CONSOLE_H
#define PROCESS_CONSOLE_H

#include "../BaseScreen.h"

class ProcessConsole : public BaseScreen{
public:
    ProcessConsole();
    ~ProcessConsole();
    void onEnabled() override;
    void display() override;
    void process() override;
    void processCommand(const std::string &command);

private:
    bool running;

    void divider(int type);
    void displayHeader();
    void displaySubHeader();
    void displayProcessHeader();
    void displayProcessBody();
};


#endif
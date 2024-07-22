#pragma once
#ifndef BASE_SCREEN_H
#define BASE_SCREEN_H

#include "AConsole.h"
#include "../Processor/Process.h"

class BaseScreen : public AConsole{
public:
    BaseScreen(std::shared_ptr<Process> process, const std::string &processName);
    void onEnabled() override;
    void display() override;
    void process() override;

    std::shared_ptr<Process> getProcess() const;

private:
    void printProcessInfo() const;
    std::shared_ptr<Process> attachedProcess;
    bool refreshed = false;
};

#endif
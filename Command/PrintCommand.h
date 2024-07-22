#pragma once
#ifndef PRINT_COMMAND_H
#define PRINT_COMMAND_H

#include "ICommand.h"
#include <string>

class PrintCommand : public ICommand{
public:
    PrintCommand(int pid, const std::string &toPrint);
    void execute() override;

private:
    std::string toPrint;
};

#endif
#include <iostream>

#include "PrintCommand.h"


PrintCommand::PrintCommand(int pid, const std::string &toPrint) : ICommand(pid, PRINT){
    this->toPrint = toPrint;
}

void PrintCommand::execute(){
    std::cout << this->toPrint << std::endl;
}
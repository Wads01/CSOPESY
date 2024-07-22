#include <iostream>

#include "BaseScreen.h"
#include "ConsoleManager.h"
#include "../Processor/CommandProcessor.h"
#include "../UI/UI_Manager.h"

BaseScreen::BaseScreen(std::shared_ptr<Process> process, const std::string &processName) : AConsole(processName), attachedProcess(process) {}

void BaseScreen::onEnabled(){
    refreshed = true;
    printProcessInfo();
    this->process();
}

void BaseScreen::display(){
    if (refreshed){
        printProcessInfo();
        refreshed = false;
    }
}

void BaseScreen::process(){
    UI_Manager& ui = UI_Manager::getInstance();
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    CommandProcessor& processor = CommandProcessor::getInstance();

    bool running = true;
    std::string command;

    while(running){
        std::cout << std::endl << "root:\\>";
        std::getline(std::cin, command);

        std::vector<std::string> tokens = processor.tokenize(command, ' ');

        if (tokens.empty())
            return;

        const std::string& command_0 = tokens[0];

        if(command_0 == "help")
        {
            std::cout << "Work In Progress" << std::endl;
        }
        else if(command_0 == "clear")
        {
            ui.clear();
            printProcessInfo();
        }
        else if(command_0 == "exit")
        {
            ui.clear();
            consoleManager->getPreviousConsole();
            running = false;
        }
        else if (command_0 == "process-smi")
        {
            printProcessInfo();
        }
        else
        {
            std::cout << "Unrecognized command:>" << command << std::endl;
        }
    }
}

void BaseScreen::printProcessInfo() const{
    std::cout << "Process: " << this->attachedProcess->getName() << std::endl;
    std::cout << "ID: " << this->attachedProcess->getPID() << std::endl;
    std::cout << std::endl;
    std::cout << "Lines of code: " << this->attachedProcess->getCommandCounter() << std::endl;
}

std::shared_ptr<Process> BaseScreen::getProcess() const{
    return attachedProcess;
}
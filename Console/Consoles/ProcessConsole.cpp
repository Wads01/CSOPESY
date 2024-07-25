#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iomanip>

#include "ProcessConsole.h"
#include "../ConsoleManager.h"
#include "../../UI/UI_Manager.h"
#include "../../Processor/Scheduler.h"
#include "../../Processor/CommandProcessor.h"
#include "../../Memory/Memory.h"

ProcessConsole::ProcessConsole() : BaseScreen(nullptr, PROCESS_CONSOLE){}

void ProcessConsole::onEnabled(){
    running = true;

    this->display();
}

void ProcessConsole::display(){
    UI_Manager& ui = UI_Manager::getInstance();
    ui.clear();
    displayHeader();
    displaySubHeader();
    displayProcessHeader();
    displayProcessBody();

    this->process();
}

void ProcessConsole::process(){
    while(running){
        std::string command;
        std::cout << std::endl << "root:\\>";
        std::getline(std::cin, command);
        processCommand(command);
    }
}

void ProcessConsole::processCommand(const std::string &command){
    UI_Manager& ui = UI_Manager::getInstance();
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    CommandProcessor& processor = CommandProcessor::getInstance();
    std::vector<std::string> tokens = processor.tokenize(command, ' ');

    if (tokens.empty())
        return;

    const std::string& command_0 = tokens[0];

    if (command_0 == "exit")
    {
        running = false;
        consoleManager->switchConsole(MAIN_CONSOLE);
    }
    else if (command_0 == "clear")
    {
        ui.clear();
        onEnabled();
    }
    else
    {
        std::cout << "Unrecognized command:>" << command << std::endl;
    }
}

void ProcessConsole::divider(int type){
    if(type == 1)
        std::cout << "+----------------------------------------------------------------------------------------------------+" << std::endl;
    else if (type == 2)
        std::cout << "+-----------------------------------------------+--------------------------+-------------------------+" << std::endl;
    else if (type == 3)
        std::cout << "+====================================================================================================+" << std::endl;
    else if (type == 4)
        std::cout << "+===============================================+==========================+=========================+" << std::endl;
}

void ProcessConsole::displayHeader(){
    UI_Manager& ui = UI_Manager::getInstance();

    divider(1);
    std::cout << "| ";
    ui.setTextColor(FOREGROUND_RED | FOREGROUND_INTENSITY, "PROCESS-SMI V01.23");
    ui.setCursorPosition(40, 1);
    ui.setTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Driver Version: 551.86");
    ui.setCursorPosition(82, 1);
    ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "CUDA Version: 12.4");
    ui.setCursorPosition(100, 1);
    std::cout << " |" << std::endl;
    divider(1);
}

void ProcessConsole::displaySubHeader(){
    UI_Manager& ui = UI_Manager::getInstance();
    Scheduler& scheduler = Scheduler::getInstance();
    Memory& memory = Memory::getInstance();

    std::vector<std::shared_ptr<Process>> runningProcesses = scheduler.getRunningProcesses();
    std::vector<std::shared_ptr<Process>> finishedProcesses = scheduler.getFinishedProcesses();

    int totalCores = scheduler.getNumCores();
    int usedCores = 0;

    for (const auto& process : runningProcesses){
        if (process->getProcessState() == Process::RUNNING)
            ++usedCores;
    }

    int cpuUtilization = (totalCores > 0) ? (usedCores * 100 / totalCores) : 0;

    size_t totalMemory = memory.getMaxSize();
    size_t usedMemory = memory.getCurrentOverallMemoryUsage();


    int memoryUtilization = (totalMemory > 0) ? (usedMemory * 100 / totalMemory) : 0;
    
    std::cout << "| ";
    ui.setCursorPosition(51, 3);
    std::cout << " | ";
    ui.setCursorPosition(100, 3);
    std::cout << " |" << std::endl;

    std::cout << "| ";
    std::cout << "Cpu Utilization: ";
    ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, std::to_string(cpuUtilization));
    std::cout << " %";
    ui.setCursorPosition(51, 4);
    std::cout << " | ";
    std::cout << "Memory Utilization: ";
    ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, std::to_string(memoryUtilization));
    std::cout << " %";
    ui.setCursorPosition(100, 4);
    std::cout << " |" << std::endl;

    std::cout << "| ";
    std::cout << "Core Usage: ";
    ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, std::to_string(usedCores));
    std::cout << " / ";
    ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, std::to_string(totalCores));
    ui.setCursorPosition(51, 5);
    std::cout << " | ";
    std::cout << "Memory Usage: ";
    ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, std::to_string(usedMemory) + " KB");
    std::cout << " / ";
    ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, std::to_string(totalMemory) + " KB");
    ui.setCursorPosition(100, 5);
    std::cout << " |" << std::endl;

    std::cout << "| ";
    ui.setCursorPosition(51, 6);
    std::cout << " | ";
    ui.setCursorPosition(100, 6);
    std::cout << " |" << std::endl;

    divider(3);
}

void ProcessConsole::displayProcessHeader(){
    UI_Manager& ui = UI_Manager::getInstance();
    
    std::cout << "| ";
    ui.setCursorPosition(33, 8);
    ui.setTextColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "RUNNING PROCESSES");
    ui.setTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, " AND ");
    ui.setTextColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "MEMORY USAGE");
    ui.setCursorPosition(100, 8);
    std::cout << " |" << std::endl;
    divider(3); 

    std::cout << "| ";
    std::cout << "Process Name";
    ui.setCursorPosition(23, 10);
    std::cout << "Timestamp";
    ui.setCursorPosition(46, 10);
    std::cout << "Core";
    ui.setCursorPosition(60, 10);
    std::cout << "Process Progress";
    ui.setCursorPosition(85, 10);
    std::cout << "Memory Usage";
    ui.setCursorPosition(100, 10);
    std::cout << " |" << std::endl;
    divider(1);
}

void ProcessConsole::displayProcessBody(){
    UI_Manager& ui = UI_Manager::getInstance();
    Scheduler& scheduler = Scheduler::getInstance();
    std::vector<std::shared_ptr<Process>> runningProcesses = scheduler.getRunningProcesses();

    int y = 12;

    for (const auto& process : runningProcesses){
        std::cout << "| ";
        std::cout << process->getName();
        ui.setCursorPosition(14, y);
        std::cout << process->getTimestamp();
        ui.setCursorPosition(47, y);
        ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, std::to_string(process->getCpuCoreID()));
        ui.setCursorPosition(63, y);
        ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, std::to_string(process->getCurrInstructions()));
        std::cout << " / ";
        ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, std::to_string(process->getMaxInstructions()));
        ui.setCursorPosition(87, y);
        ui.setTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, std::to_string(process->getMemRequired()) + " KB");
        ui.setCursorPosition(100, y);
        std::cout << " |" << std::endl;

        y += 1;
    }

    divider(1);

}

ProcessConsole::~ProcessConsole() {}
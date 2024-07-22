#include <iostream>
#include <chrono>

#include "ConsoleManager.h"
#include "../Processor/Scheduler.h"

UI_Manager& ui = UI_Manager::getInstance();
ConsoleManager* ConsoleManager::sharedInstance = nullptr;

ConsoleManager* ConsoleManager::getInstance(){
    if (!sharedInstance)
        initialize();

    return sharedInstance;
}

void ConsoleManager::initialize(){
    if (!sharedInstance)
        sharedInstance = new ConsoleManager();
}

void ConsoleManager::destroy(){
    if (sharedInstance){
        delete sharedInstance;
        sharedInstance = nullptr;
    }
}


ConsoleManager::ConsoleManager(){
    running = true;

    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    const std::shared_ptr<MainConsole> mainConsole = std::make_shared<MainConsole>();
    const std::shared_ptr<MarqueeConsole> marqueeConsole = std::make_shared<MarqueeConsole>();
    const std::shared_ptr<ProcessConsole> processConsole = std::make_shared<ProcessConsole>();
    

    consoleTable[MAIN_CONSOLE] = mainConsole;
    consoleTable[MARQUEE_CONSOLE] = marqueeConsole;
    consoleTable[PROCESS_CONSOLE] = processConsole;
    
    switchConsole(MAIN_CONSOLE);
}

void ConsoleManager::drawConsole() const{
    if (currConsole)
        currConsole->display();
    else
        std::cout << "Error: There is no assigned console." << std::endl;
}

void ConsoleManager::process() const{
    if (currConsole)
        currConsole->process();
    else
        std::cout << "Error: There is no assigned console." << std::endl;
}

void ConsoleManager::switchConsole(const std::string &consoleName) {
    auto it = consoleTable.find(consoleName);
    if (it != consoleTable.end()){
        ui.clear();
        prevConsole = currConsole;
        currConsole = it->second;
        currConsole->onEnabled();
    }
    else{
        std::cout << "Error: Console Name '" << consoleName << "' Cannot Be Found." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

bool ConsoleManager::registerScreen(std::shared_ptr<BaseScreen> screenReference){
    auto it = consoleTable.find(screenReference->getName());
    if (it != consoleTable.end())
        return false;
    else{
        consoleTable[screenReference->getName()] = screenReference;
        return true;
    }
}

void ConsoleManager::unregisterScreen(const std::string &screenName){
    auto it = consoleTable.find(screenName);
    if (it != consoleTable.end())
        consoleTable.erase(screenName);
    else
        std::cout << "Error: Console Name '" << screenName << "' Cannot Be Found." << std::endl;
}

void ConsoleManager::listScreens() const {
    Scheduler& scheduler = Scheduler::getInstance();

    std::vector<std::shared_ptr<Process>> runningProcesses = scheduler.getRunningProcesses();
    std::vector<std::shared_ptr<Process>> finishedProcesses = scheduler.getFinishedProcesses();

    int totalCores = scheduler.getNumCores();
    int usedCores = 0;

    for (const auto& process : runningProcesses) {
        if (process->getProcessState() == Process::RUNNING)
            ++usedCores;
    }

    int availableCores = totalCores - usedCores;
    double cpuUtilization = 0.0;
    if (totalCores > 0)
        cpuUtilization = static_cast<double>(usedCores) / totalCores * 100.0;

    std::cout << "---------------------------" << std::endl;

    std::cout << "CPU Utilization: " << cpuUtilization << "%" << std::endl;
    std::cout << "Cores Used: " << usedCores << std::endl;
    std::cout << "Cores Available: " << availableCores << std::endl;

    std::cout << std::endl << std::endl;
    
    std::cout << "Running Processes:" << std::endl;
    for (const auto& process : runningProcesses){
        std::cout << "Process: " << process->getName() << " (" << process->getTimestamp() << ") | Core: " << process->getCpuCoreID() << " | ";
        std::cout << process->getCurrInstructions() << " / " << process->getMaxInstructions() << std::endl;
    }

    std::cout << std::endl << std::endl;

    std::cout << "Finished Processes:" << std::endl;
    for (const auto& process : finishedProcesses){
        std::cout << "Process: " << process->getName() << " (" << process->getTimestamp() << ") | Core: " << "Finished" << " | ";
        std::cout << process->getCurrInstructions() << " / " << process->getMaxInstructions() << std::endl;
    }

    std::cout << "---------------------------" << std::endl;
}

void ConsoleManager::listScreens(std::ofstream& logfile) const{
    Scheduler& scheduler = Scheduler::getInstance();

    std::vector<std::shared_ptr<Process>> runningProcesses = scheduler.getRunningProcesses();
    std::vector<std::shared_ptr<Process>> finishedProcesses = scheduler.getFinishedProcesses();

    int totalCores = scheduler.getNumCores();
    int usedCores = 0;

    for (const auto& process : runningProcesses){
        if (process->getProcessState() == Process::RUNNING)
            ++usedCores;
    }

    int availableCores = totalCores - usedCores;

    double cpuUtilization = 0.0;
    if (totalCores > 0)
        cpuUtilization = static_cast<double>(usedCores) / totalCores * 100.0;


    logfile << "---------------------------" << std::endl;

    logfile << "CPU Utilization: " << cpuUtilization << "%" << std::endl;
    logfile << "Cores Used: " << usedCores << std::endl;
    logfile << "Cores Available: " << availableCores << std::endl;

    logfile << std::endl << std::endl;

    logfile << "Running Processes:" << std::endl;
    for (const auto& process : runningProcesses) {
        logfile << "Process: " << process->getName() << " (" << process->getTimestamp() << ") | Core: " << process->getCpuCoreID() << " | ";
        logfile << process->getCurrInstructions() << " / " << process->getMaxInstructions() << std::endl;
    }

    logfile << std::endl << std::endl;

    logfile << "Finished Processes:" << std::endl;
    for (const auto& process : finishedProcesses) {
        logfile << "Process: " << process->getName() << " (" << process->getTimestamp() << ") | Core: " << "Finished" << " | ";
        logfile << process->getCurrInstructions() << " / " << process->getMaxInstructions() << std::endl;
    }

    logfile << "---------------------------" << std::endl;
}

void ConsoleManager::getPreviousConsole(){
    if (prevConsole){
        currConsole = prevConsole;
        currConsole->onEnabled();
        prevConsole = nullptr;
    }
}

void ConsoleManager::exit(){
    running = false;
}

bool ConsoleManager::isRunning() const{
    return running;
}

HANDLE ConsoleManager::getConsoleHandle() const{
    return consoleHandle;
}
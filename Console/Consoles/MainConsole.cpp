#include <iostream>
#include <random>
#include <chrono>

#include "MainConsole.h"
#include "../ConsoleManager.h"
#include "../../UI/UI_Manager.h"
#include "../../Processor/Scheduler.h"
#include "../../Processor/CommandProcessor.h"

MainConsole::MainConsole() : BaseScreen(nullptr, MAIN_CONSOLE) {}

void MainConsole::onEnabled(){
    UI_Manager& ui = UI_Manager::getInstance();
    ui.clear();
    ui.printMainHeader();
    ui.printMainWelcome();
}

void MainConsole::display(){
    std::cout << "Main Console Display" << std::endl;
}

void MainConsole::process(){
    std::string command;
    std::cout << std::endl << "Enter a command:>";
    std::getline(std::cin, command);

    if(!initialized)
        initialization(command);
    else
        processCommand(command);
}

void MainConsole::processCommand(const std::string &command){
    UI_Manager& ui = UI_Manager::getInstance();
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    CommandProcessor& processor = CommandProcessor::getInstance();
    std::vector<std::string> tokens = processor.tokenize(command, ' ');

    if (tokens.empty())
        return;

    const std::string& command_0 = tokens[0];

    if (command_0 == "help")
    {
        UI_Manager& ui = UI_Manager::getInstance();
        ui.printMainCommandList();
    }
    else if (command_0 == "clear")
    {
        ui.clear();
        onEnabled();
    }
    else if (command_0 == "marquee")
    {
        consoleManager->switchConsole(MARQUEE_CONSOLE);
        ui.clear();
        onEnabled();
    }
    else if (command_0 == "process-smi")
    {
        consoleManager->switchConsole(PROCESS_CONSOLE);
        ui.clear();
        onEnabled();
    }
    else if (command_0 == "vmstat")
    {
        vmStat(memory->getAllocator()->getName());
    }
    else if (command_0 == "view-config")
    {
        std::cout << std::endl;
        scheduler->getInfo();
        std::cout << std::endl << std::endl;
        memory->getInfo();
    }
    else if (command_0 == "screen")
    {
        if(tokens.size() == 3)
        {
            if(tokens[1] == "-s")
            {
                // Generate PID
                int pid = ui.generatePID();

                // Set requirement flags
                Process::RequirementFlags reqFlags = {true, 1, true, 512};

                //Get current timestamp
                std::string timestamp = ui.generateTimestamp();

                // Create Process
                std::shared_ptr<Process> newProcess = std::make_shared<Process>(pid, tokens[2], reqFlags, timestamp, scheduler->getMinInstructions(), scheduler->getMaxInstructions(), scheduler->getMinMem(), scheduler->getMaxMem(), scheduler->getMinPage(), scheduler->getMaxPage());

                // Create Screen with Process
                std::shared_ptr<BaseScreen> newScreen = std::make_shared<BaseScreen>(newProcess, tokens[2]);

                if (consoleManager->registerScreen(newScreen)){
                    std::cout << "Screen Name: '" << tokens[2] << "' successfully created." << std::endl;
                    consoleManager->switchConsole(tokens[2]);
                    scheduler->addProcess(newProcess);

                    ui.clear();
                    onEnabled();
                }
                else
                    std::cout << "Error: Screen Name '" << tokens[2] << "' Already Exists. Use A Different Name." << std::endl;
            }
            else if(tokens[1] == "-r")
            {
                consoleManager->switchConsole(tokens[2]);
            }
            else if(tokens[1] == "-d")
            {
                consoleManager->unregisterScreen(tokens[2]);
                std::cout << "Screen Name: '" << tokens[2] << "' successfully removed." << std::endl;
            }
        }
        else if(tokens.size() == 2)
        {
            if(tokens[1] == "-ls")
            {
                consoleManager->listScreens();
            }
        }
        else if (tokens.size() == 1)
        {
            printDetails();
        }
    }
    else if (command_0 == "scheduler-test")
    {

        if(runScheduler)
            std::cout << "Scheduler Already Running" << std::endl;
        else{
            runScheduler = true;
            schedulerStartThread = std::thread(&MainConsole::schedulerStart, this);
            std::cout << "Scheduler Started" << std::endl;
        }
    }
    else if (command_0 == "scheduler-stop")
    {
        if(!runScheduler)
            std::cout << "Scheduler Is Not Running" << std::endl;
        else{
            schedulerStop();
            std::cout << "Scheduler Stoppped" << std::endl;
        }
    }
    else if (command_0 == "report-util")
    {
        try{
            std::ofstream logfile("csopesy-log.txt", std::ios_base::app);
            if (!logfile.is_open()){
                std::cerr << "Error opening log file: csopesy-log.txt" << std::endl;
                return;
            }

            ConsoleManager* consoleManager = ConsoleManager::getInstance();
            consoleManager->listScreens(logfile);

            logfile.close();
            std::cout << "Report generated and saved to csopesy-log.txt" << std::endl;
        }
        catch (const std::exception& e){
            std::cerr << "Error generating report: " << e.what() << std::endl;
        }
    }
    else if (command_0 == "exit")
    {

        scheduler->shutdown();
        consoleManager->exit();
        std::cout << "Program Terminated Successfully." << std::endl;
    }
    else
    {
        std::cout << "Unrecognized command:>" << command << std::endl;
    }

    tokens.clear();
}

void MainConsole::schedulerStart(){
    UI_Manager& ui = UI_Manager::getInstance();
    ConsoleManager* consoleManager = ConsoleManager::getInstance();

    while(runScheduler){

        int pid = ui.generatePID();
        Process::RequirementFlags reqFlags = { true, 1, true, 512 };
        std::string timestamp = ui.generateTimestamp();
        std::string name = "p_" + std::to_string(pid);

        std::shared_ptr<Process> newProcess = std::make_shared<Process>(pid, name, reqFlags, timestamp, scheduler->getMinInstructions(), scheduler->getMaxInstructions(), scheduler->getMinMem(), scheduler->getMaxMem(), scheduler->getMinPage(), scheduler->getMaxPage());

        std::shared_ptr<BaseScreen> newScreen = std::make_shared<BaseScreen>(newProcess, name);
        consoleManager->registerScreen(newScreen);
        scheduler->addProcess(newProcess);

        double frequency = scheduler->getBatchProcessFrequency();

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(frequency * 1000)));
    }
}

void MainConsole::schedulerStop(){
    runScheduler = false;

    if (schedulerStartThread.joinable())
        schedulerStartThread.join();
}


void MainConsole::initialization(const std::string &command){
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    UI_Manager& ui = UI_Manager::getInstance();

    if(command == "initialize")
    {
        scheduler = &Scheduler::getInstance();

        std::cout << "Initializing Operating System..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Initialize Scheduler
        try{
            scheduler->readConfigFile("config.txt");
            std::cout << "Scheduler configuration loaded successfully." << std::endl;
        }
        catch (const std::exception& e){
            std::cerr << "Error initializing scheduler: " << e.what() << std::endl;
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Initialize Memory
        memory = &Memory::getInstance();
        try {
            memory->readConfigFile("config.txt");
            std::cout << "Memory configuration loaded successfully." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error initializing memory: " << e.what() << std::endl;
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Equip Memory to Scheduler
        scheduler->setMemory(*memory);

        std::cout << "System Successfully Initiailized" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        ui.clear();
        onEnabled();

        initialized = true;
        std::thread schedulerThread(&Scheduler::run, scheduler);
        schedulerThread.detach();
    }
    else if (command == "exit")
    {
        consoleManager->exit();
        std::cout << "Program Terminated." << std::endl;
    }
    else
        std::cout << "Please type 'initialize' to start the program or 'exit' to terminate the program." << std::endl;
}

void MainConsole::printDetails(){
    UI_Manager& ui = UI_Manager::getInstance();
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Screen Name: Main Console" << std::endl;
    std::cout << "Screen ID: 1000" << std::endl;
    std::cout << "Available Number of Commands: 13" << std::endl;
    std::cout << "Current Time: " << ui.generateTimestamp() << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}

void MainConsole::vmStat(const std::string &allocator){
    UI_Manager& ui = UI_Manager::getInstance();
    size_t totalMemory = memory->getMaxSize();
    size_t usedMemory = memory->getCurrentOverallMemoryUsage();
    int idleTicks = scheduler->idleCPUTicks;
    int activeTicks = scheduler->activeCPUTicks;
    int totalTicks = idleTicks + activeTicks;

    std::cout << "+----------------------------------------+" << std::endl;
    std::cout << "|              VM STATISTICS             |" << std::endl;
    std::cout << "+----------------------------------------+" << std::endl;
    std::cout << " Timestamp: " << ui.generateTimestamp() << std::endl;
    std::cout << " Allocator: " << memory->getAllocator()->getName() << std::endl;
    std::cout << std::endl;
    std::cout << "      Total Memory:           " << totalMemory << " KB" << std::endl;
    std::cout << "      Used Memory:            " << usedMemory << " KB" << std::endl;
    std::cout << "      Total CPU Ticks:        " << totalTicks << std::endl;
    std::cout << "      Active CPU Ticks:       " << activeTicks << std::endl;
    std::cout << "      Idle CPU Ticks:         " << idleTicks << std::endl;

    if (allocator == "PagingMemoryAllocator"){
        auto pagingAllocator = dynamic_cast<PagingMemoryAllocator*>(memory->getAllocator());
        if (pagingAllocator){
            size_t activeMemory = pagingAllocator->getActiveMem();
            size_t inactiveMemory = usedMemory - activeMemory;
            int numPagedIn = scheduler->numPagedIn;
            int numPagedOut = scheduler->numPagedOut;

            std::cout << "      Active Memory:          " << activeMemory << " KB" << std::endl;
            std::cout << "      Inactive Memory:        " << inactiveMemory << " KB" << std::endl;
            std::cout << "      Pages In:               " << numPagedIn << std::endl;
            std::cout << "      Pages Out:              " << numPagedOut << std::endl;
        }
    }

    else if (allocator == "FlatMemoryAllocator"){
        auto flatAllocator = dynamic_cast<FlatMemoryAllocator*>(memory->getAllocator());
        if (flatAllocator){
            size_t activeMemory = flatAllocator->getActiveMem();
            size_t inactiveMemory = usedMemory - activeMemory;

            std::cout << "      Active Memory:          " << activeMemory << " KB" << std::endl;
            std::cout << "      Inactive Memory:        " << inactiveMemory << " KB" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "+----------------------------------------+" << std::endl;
}
MainConsole::~MainConsole(){
    scheduler = nullptr;
}
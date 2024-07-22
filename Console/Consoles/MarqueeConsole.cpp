#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

#include "MarqueeConsole.h"
#include "../ConsoleManager.h"
#include "../../UI/UI_Manager.h"
#include "../../Processor/Scheduler.h"
#include "../../Processor/CommandProcessor.h"

MarqueeConsole::MarqueeConsole() : BaseScreen(nullptr, MARQUEE_CONSOLE), running(true) {}

MarqueeConsole::~MarqueeConsole(){
    running.store(false);
    if (marqueeThread.joinable()) {
        marqueeThread.join();
    }
    if (inputThread.joinable()) {
        inputThread.join();
    }
}

void MarqueeConsole::onEnabled(){
    reset(); 

    active = true;
    running.store(true);
    marqueeThread = std::thread(&MarqueeConsole::marquee, this, "Hello World In Marquee!", std::ref(running));
    inputThread = std::thread(&MarqueeConsole::pollInput, this, std::ref(running));
    this->process();
}

void MarqueeConsole::display(){
    std::cout << "Marquee Console Display" << std::endl;
}

void MarqueeConsole::process(){
    while(active){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void MarqueeConsole::processMarqueeCommand(const std::string &command){
    ConsoleManager* consoleManager = ConsoleManager::getInstance();
    CommandProcessor& processor = CommandProcessor::getInstance();
    std::vector<std::string> tokens = processor.tokenize(command, ' ');

    if (tokens.empty())
        return;

    const std::string& command_0 = tokens[0];

    if (command_0 == "exit")
    {
        stackInput.clear();
        running.store(false);
        active = false;
        consoleManager->switchConsole(MAIN_CONSOLE);
    }
}

void MarqueeConsole::marquee(const std::string &text, std::atomic<bool>& running){
    int width, height;
    UI_Manager::getInstance().getConsoleSize(width, height);
    int x = 0, y = 4, dx = 1, dy = 1;

    while (running){
        UI_Manager::getInstance().clear();

        std::cout << "+----------------------------------------------------------------------------------------------------+" << std::endl;
        std::cout << "|";
        UI_Manager::getInstance().setCursorPosition(37, 1);
        std::cout << "! Displaying A Marquee Console !";
        UI_Manager::getInstance().setCursorPosition(101, 1);
        std::cout << "|" << std::endl;
        std::cout << "+----------------------------------------------------------------------------------------------------+" << std::endl;

        UI_Manager::getInstance().setCursorPosition(x, y);
        std::cout << text;

        x += dx;
        y += dy;

        if (x <= 0 || x >= width - static_cast<int>(text.length()))
            dx = -dx;

        if (y <= 4 || y >= height - 6)
            dy = -dy;

        {
            std::lock_guard<std::mutex> lock(mtx);
            cv.notify_one();
        }

        UI_Manager::getInstance().setCursorPosition(0, height - 5);

        std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }
}

void MarqueeConsole::pollInput(std::atomic<bool>& running){
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRecord;
    DWORD events;
    DWORD bytesRead;

    while (running.load()){
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock);

        displayPreviousCommands(previousCommands);
        std::cout << "Enter Marquee Command>";

        for (auto key : stackInput)
            std::cout << key;

        if (PeekConsoleInput(hInput, &inputRecord, 1, &events) && events > 0){
            if (ReadConsoleInput(hInput, &inputRecord, 1, &bytesRead) && inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown){
                WORD virtualKey = inputRecord.Event.KeyEvent.wVirtualKeyCode;
                char key = inputRecord.Event.KeyEvent.uChar.AsciiChar;
                if (virtualKey == VK_RETURN){
                    std::string stringInput(stackInput.begin(), stackInput.end());
                    std::string prevCommand = "Enter Marquee Command>" + stringInput;

                    previousCommands.push_back(prevCommand);
                    if (previousCommands.size() > 4)
                        previousCommands.pop_front();

                    processMarqueeCommand(stringInput);

                    stackInput.clear();
                }
                else if (virtualKey == VK_BACK){
                    if (!stackInput.empty())
                        stackInput.pop_back();
                }
                else
                    stackInput.push_back(key);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }
}

void MarqueeConsole::displayPreviousCommands(const std::deque<std::string>& previousCommands){
    for (const auto& command : previousCommands)
        std::cout << command << std::endl;
}

void MarqueeConsole::reset(){
    running.store(false);
    
    if (marqueeThread.joinable()){
        marqueeThread.join();
    }
    if (inputThread.joinable()){
        inputThread.join();
    }

    stackInput.clear();
    previousCommands.clear();
}

std::mutex& MarqueeConsole::getMutex(){
    return mtx;
}

std::condition_variable& MarqueeConsole::getCV(){
    return cv;
}
#pragma once
#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <string>
#include <iostream>

class UI_Manager{
public:
    static UI_Manager& getInstance();
    static void initialize();
    static void destroy();

    void clear();
    void printMainHeader();
    void printMainWelcome();
    void printMainCommandList();
    void printMessage(const std::string &message);
    std::string generateTimestamp();

    void setCursorPosition(int x, int y);
    void getConsoleSize(int &width, int &height);
    void setTextColor(int color, const std::string &text);

    static int generatePID();

private:
    UI_Manager() = default;
    UI_Manager(const UI_Manager&) = delete;
    UI_Manager& operator=(const UI_Manager&) = delete;
    static UI_Manager* sharedInstance;
    
    static int pid_generate;
};

#endif
#pragma once
#ifndef A_CONSOLE_H
#define A_CONSOLE_H

#include <string>

class AConsole{
public:
    AConsole(std::string name) : name(name) {}
    virtual ~AConsole() = default;

    std::string getName() const { return name; }
    virtual void onEnabled() = 0;
    virtual void display() = 0;
    virtual void process() = 0;

protected:
    std::string name;
};

#endif
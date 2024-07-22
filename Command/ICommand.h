#pragma once
#ifndef ICommand_H
#define ICommand_H

class ICommand{
public:
    enum CommandType
    {
        IO,
        PRINT,
        SLEEP,
        ADD,
        SUB,
        DIV,
        MUL,
        MOD
    };

    ICommand(int pid, CommandType commandType);
    CommandType getCommandType();
    virtual void execute() = 0;

protected:
    int pid;
    CommandType commandType;

};

inline ICommand::CommandType ICommand::getCommandType(){
    return this->commandType;
}

#endif
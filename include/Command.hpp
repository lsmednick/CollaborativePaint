/** 
 *  @file   Command.hpp 
 *  @brief  Represents an actionable command by the user.
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/
#ifndef COMMAND_HPP
#define COMMAND_HPP

// Include standard library C++ libraries.
#include <string>

class Command {
private:
    std::string m_commandDescription;
public:
    // Constructor for a command
    Command();

    // Destructor for a command
    virtual ~Command();

    // Returns true or false if the command was able to successfully
    // execute.
    // If it executes successfully, then that command is added to the
    // undo stack.
    virtual bool execute() = 0;

    // Returns true or false if the command was successfully undone.
    // If it executes successfully, then that command is added to the
    // redo stack.
    virtual bool undo() = 0;

    // Get the affected pixel X value
    virtual int getPixelX() = 0;

    // Get the affected pixel Y value
    virtual int getPixelY() = 0;

};


#endif
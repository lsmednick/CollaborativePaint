/**
 *  @file   FillDisplay.hpp
 *  @brief  Fill display actions interface.
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/
#ifndef FILLDISPLAY_H
#define FILLDISPLAY_H

// Include standard library C++ libraries.
#include <string>
// Project header files
#include "Command.hpp"
#include "App.hpp"

class FillDisplay : public Command {
    // App to operate upon
    App *minipaint;

    // Color to fill app display with
    sf::Color color;

    // X coordinate of mouse
    int m_x;

    // Y coordinate of mouse
    int m_y;

    // Prior pixel values for all pixels in the canvas prior to fill
    std::map<std::pair<int, int>, sf::Color> priorPixelValues;

public:
    // Constructor for FillDisplay command
    FillDisplay(App *app, int color);

    // Execute a fill display operation
    bool execute() override;

    // Undo a fill display operation
    bool undo() override;

    // Get mouse x coordinate
    int getPixelX() override;

    // Get mouse y coordinate
    int getPixelY() override;

    // Destructor
    virtual ~FillDisplay();
};

#endif
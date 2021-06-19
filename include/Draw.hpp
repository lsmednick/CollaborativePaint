/** 
 *  @file   Draw.hpp 
 *  @brief  Drawing actions interface. 
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/
#ifndef DRAW_H
#define DRAW_H

// Include standard library C++ libraries.
#include <string>
// Project header files
#include "Command.hpp"
#include "App.hpp"


class Draw : public Command {

    // Minipaint app object to operate upon
    App *minipaint;

    // x coordinate
    int m_x;

    // y coordinate
    int m_y;

    // Size of brush stroke
    int size;

    // Color of brush
    sf::Color color;

    // Prior color of paint brush
    sf::Color prior_color;

    // Prior color of pixel, or pixel and surrounding pixels if brush diameter > 1
    std::map<std::pair<int, int>, sf::Color> priorDrawnPixels;

    // Paint function to paint with
    std::map<std::pair<int, int>, sf::Color> (*paintFunc)(App *, sf::Color color, int size, int m_x, int m_y);

public:
    // Constructor
    Draw(App *app);

    // Second constructor, with parameters
    Draw(App *app, int x, int y, sf::Color color, int size);

    // Execute method
    bool execute() override;

    // Undo method
    bool undo() override;

    // Get pixel x coordinate
    int getPixelX() override;

    // Get pixel y coordinate
    int getPixelY() override;

    // Destructor
    virtual ~Draw();
};

#endif
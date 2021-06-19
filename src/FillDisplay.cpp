/**
 *  @file   FillDisplay.cpp
 *  @brief  FillDisplay implementation, all fill screen actions are commands.
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/

// Include our Third-Party SFML header
#include <SFML/Graphics/Color.hpp>
// Include standard library C++ libraries.
// Project header files
#include <iostream>
#include <App.hpp>
#include "FillDisplay.hpp"


/*! \brief 	FillDisplay object stores the color information for this command,
 * as well as the mouse position.
 * FillDisplay also contains an attribute called priorScreen, which is a structure mapping pixel coordinates
 * to the prior color value of each pixel before the FillDisplay command was executed.
 * @param app the app to act upon
 * @param color an int representing the fill color value
*/
FillDisplay::FillDisplay(App *app, int color) {
    FillDisplay::minipaint = app;
    FillDisplay::color = sf::Color(color);
    FillDisplay::m_x = app->mouseX;
    FillDisplay::m_y = app->mouseY;

}

/*! \brief 	Execute a FillDisplay command, filling the display screen with the current paint color
 * @return bool representing success of execution
*
*/
bool FillDisplay::execute() {
    std::cout << "executing fill screen operation - this may take a moment..." << std::endl;

    bool success = true;
    // Iterate through every pixel in the screen
    for (int i = 0; i < minipaint->GetDisplayDimensions().x; i++) {
        for (int j = 0; j < minipaint->GetDisplayDimensions().y; j++) {
            priorPixelValues[std::make_pair(i, j)] = minipaint->GetImage().getPixel(i, j);
            minipaint->GetImage().setPixel(i, j, color);
            if (minipaint->GetImage().getPixel(i, j) != color) {
                success = false;
            }
        }
    }
    return success;
}

/*! \brief Get pixel X value of mouse upon this action
 * @return int - the value of the mouse's original x coordinate.
 *
 */
int FillDisplay::getPixelX() {
    return m_x;
}

/*! \brief Get pixel Y value of mouse upon this action
 * @return int - the value of the mouse's original y coordinate.
 *
 */
int FillDisplay::getPixelY() {
    return m_y;
}

/*! \brief 	Undo this FillDisplay command by restoring the prior set of pixel colors affected by this command.
 * @return bool representing success of undo
 *
*/
bool FillDisplay::undo() {
    std::cout << "undoing fill screen operation - this may take a moment..." << std::endl;
    bool success = true;

    for (auto &priorPixel : priorPixelValues) {
        minipaint->GetImage().setPixel(priorPixel.first.first, priorPixel.first.second,
                                       priorPixel.second);
        if (minipaint->GetImage().getPixel(priorPixel.first.first, priorPixel.first.second) != priorPixel.second) {
            success = false;
        }
    }
    return success;
}


/*! \brief 	Delete this FillDisplay object.
 *
*/
FillDisplay::~FillDisplay() = default;
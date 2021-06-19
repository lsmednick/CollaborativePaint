/** 
 *  @file   Draw.cpp 
 *  @brief  Draw implementation, all drawing actions are commands. 
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/

// Include our Third-Party SFML header
#include <SFML/Graphics/Color.hpp>
// Include standard library C++ libraries.
// Project header files
#include "Draw.hpp"

/*! \brief 	Draw object stores the pixel position, color information, brushstroke size,
 * prior color at the given pixel, and paint function for this command.
 * @param app the app to draw upon
*
*/
Draw::Draw(App *app) {
    Draw::minipaint = app;
    Draw::m_x = app->mouseX;
    Draw::m_y = app->mouseY;
    Draw::size = app->strokeSize;
    Draw::color = app->m_color;
    Draw::prior_color = app->GetImage().getPixel(m_x, m_y);
    Draw::paintFunc = app->m_paintFunc;
}

/*! \brief 	Draw object stores the pixel position, color information, brushstroke size,
 * prior color at the given pixel, and paint function for this command.
 * @param app the app to draw upon
 * @param x the x-coordinate of the douse
 * @param y the y-coordinate of the drawing
 * @param color the color to draw in
 * @param size the size of the brushstroke
 *
*
*/
Draw::Draw(App *app, int x, int y, sf::Color color, int size) {
    Draw::minipaint = app;
    Draw::m_x = x;
    Draw::m_y = y;
    Draw::size = size;
    Draw::color = color;
    Draw::prior_color = app->GetImage().getPixel(m_x, m_y);
    Draw::paintFunc = app->m_paintFunc;
}

/*! \brief 	Execute a Draw command with the App's specified paint function.
 * @return bool representing success of execute function
*
*/
bool Draw::execute() {
    priorDrawnPixels = paintFunc(minipaint, color, size, m_x, m_y);
    return minipaint->GetImage().getPixel(m_x, m_y) == color;
}

/*! \brief Return the value of the drawn pixel's x coordinate.
 * @return int - the x coordinate
 */
int Draw::getPixelX() {
    return m_x;
}

/*! \brief Return the value of the drawn pixel's y coordinate.
 * @return y - the y coordinate
 */
int Draw::getPixelY() {
    return m_y;
}

/*! \brief 	Undo this Draw command by restoring the prior set of pixel colors affected by this command.
 * @return bool representing success of undo function
*
*/
bool Draw::undo() {
    for (auto &priorDrawnPixel : priorDrawnPixels) {
        minipaint->GetImage().setPixel(priorDrawnPixel.first.first, priorDrawnPixel.first.second,
                                       priorDrawnPixel.second);
    }
    return minipaint->GetImage().getPixel(m_x, m_y) == prior_color;
}

/*! \brief 	Delete this Draw object.
*
*/
Draw::~Draw() {}

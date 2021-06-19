/** 
 *  @file   App.cpp 
 *  @brief  Class for running paint app
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/

// Include our Third-Party SFML header
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
// Include standard library C++ libraries.
#include <cassert>
// Project header files
#include "App.hpp"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define CANVAS_WINDOW_HEIGHT 850

/*! \brief
 * Initialize an App with its constructor which has no parameters.
 * Initialize attributes with specific values.
*/
App::App() {

    // Function pointers
    void (*m_initFunc)(void) = nullptr;
    void (*m_updateFunc)(void) = nullptr;
    void (*m_drawFunc)(void) = nullptr;
    std::map<std::pair<int, int>, sf::Color> (*m_paintFunc)(sf::Color, int) = nullptr;

    // Drawing variables
    App::mouseX = 0;
    App::mouseY = 0;
    App::m_canvas = sf::Color::White;
    App::m_color = sf::Color::Black;
    App::strokeSize = 1;

    // Canvas variables
    App::m_window = nullptr;
    App::m_gui = nullptr;
    App::m_image = new sf::Image;
    App::m_sprite = new sf::Sprite;
    App::m_texture = new sf::Texture;
    App::m_displayOffset = 150;
}

/*! \brief
 *		Execute a command on a single pixel, and store it in a stack of pixels affected by this user action.
 *		Reset the "redo" stack so that no actions can be redone after this command is executed.
* @param command the Command to be executed
 * @return void
*/
void App::ExecuteCommand(Command *command) {
    if (command->execute()) {
        m_paintedPixels.push_back(command);
        m_redo = std::stack<std::list<Command *>>();
    }
}

/*! \brief
 * Add all the pixels affected by a single user action, e.g
 * . click-and-drag brushstroke, to the "undo" stack.
 * @return void
*
*/
void App::AddCommand() {
    m_undo.push(m_paintedPixels);
}

/*! \brief
 * Undo a prior command, e.g. a brushstroke or filling the canvas with a color. Push the undone command
 * to the "redo" stack.
 * @return void
*
*/
void App::UndoCommand() {
    if (m_undo.empty()) {
        return;
    }
    std::list<Command *> userAction = m_undo.top();
    std::list<Command *>::reverse_iterator it;
    for (it = userAction.rbegin(); it != userAction.rend(); ++it) {
        mouseX = (*it)->getPixelX();
        mouseY = (*it)->getPixelY();
        (*it)->undo();
    }
    m_redo.push(userAction);
    m_undo.pop();
}

/*! \brief
 * Redo an undone command, e.g. a brushstroke or filling the canvas with a color. Push the redone command
 * to the "undo" stack.
 * @return void
*/
void App::RedoCommand() {
    if (m_redo.empty()) {
        return;
    }
    std::list<Command *> userAction = m_redo.top();
    std::list<Command *>::iterator it;
    for (it = userAction.begin(); it != userAction.end(); ++it) {
        mouseX = (*it)->getPixelX();
        mouseY = (*it)->getPixelY();
        (*it)->execute();
    }
    m_undo.push(userAction);
    m_redo.pop();
}

/*!
 * \brief Fill the display screen with the current color, and record this action as a command in the undo stack.
 *
 * @param command the FillDisplay command to be completed
 * @return void
 */
void App::FillDisplay(Command *command) {
    if (command->execute()) {
        std::list<Command *> fillCommand{command};
        m_undo.push(fillCommand);
        m_redo = std::stack<std::list<Command *>>();
    }
}

/*! \brief 	Return a reference to our m_image, so that
*		we do not have to publicly expose it.
 *		@return the Image of this app
*
*/
sf::Image &App::GetImage() {
    return *m_image;
}

/*! \brief 	Return a reference to our m_Texture so that
*		we do not have to publicly expose it.
 *		@return the Texture of this app
*
*/
sf::Texture &App::GetTexture() {
    return *m_texture;
}

/*! \brief 	Return a reference to our m_gui, corresponding to the GUI button window, so that we
*		do not have to publicly expose it.
 *		@return RenderWindow of the GUI
*
*/
sf::RenderWindow &App::GetGui() {
    return *m_gui;
}

/*! \brief 	Return the sprite to display information on the screen.
 * @return the sprite of this app
*
*/
sf::Sprite &App::GetSprite() {
    return *m_sprite;
}

/*! \brief Return an integer representing the pixel offset of the GUI buttons vs. the display window.
 * This data is necessary in order to prevent a user's click on a GUI button from resulting in a paint stroke
 * on the display window.
 * @return int representing the y-coordinate offset for drawing on the display window.
 */
int App::GetDisplayOffset() {
    return m_displayOffset;
}

/*! \brief 	Return a reference to our m_window (display window) so that we
*		do not have to publicly expose it.
 *		@return RenderWindow representing the display window
*
*/
sf::RenderWindow &App::GetDisplayWindow() {
    return *m_window;
}

/*! \brief Return a Vector2u SFML vector representing the (x, y) dimensions of the display window.
 * @return Vector2u SFML object storing the x, y dimensions of the window
 */
sf::Vector2u App::GetDisplayDimensions() {
    return m_window->getSize();
}

/*! \brief 	Destroy we manually call at end of our program.
 * @return void
*
*/
void App::Destroy() {
    delete m_image;
    delete m_sprite;
    delete m_texture;
}

/*! \brief 	Initializes the App and sets up the main
*		rendering window(i.e. our canvas.)
 *		@param void (*initFunction)(void) - initialization function
 *		@return void
*/
void App::Init(void (*initFunction)(void)) {

    // Setup the context
    sf::ContextSettings settings(24, 8, 4, 2, 2);
    // Create a rendering window where we can draw an image on
    m_window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, CANVAS_WINDOW_HEIGHT),
                                    "Mini-Paint Lab beta 2.0", sf::Style::Default,
                                    settings);
    // Set the sync rate after creating a window
    // Refreshes window at same rate as the monitor
    m_window->setVerticalSyncEnabled(true);
    // Create a GUI window to draw to
    m_gui = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                                 "GUI Window", sf::Style::Default, settings);
    m_gui->setVerticalSyncEnabled(true);
    m_gui->setActive(true);


    // Create an image which stores the pixels we will update
    m_image->create(WINDOW_WIDTH, CANVAS_WINDOW_HEIGHT, m_canvas);
    assert(m_image != nullptr && "m_image != nullptr");
    // Create a texture which lives in the GPU and will render our image
    m_texture->loadFromImage(*m_image);
    assert(m_texture != nullptr && "m_texture != nullptr");
    // Create a sprite which is the entity that can be textured
    m_sprite->setTexture(*m_texture);
    assert(m_sprite != nullptr && "m_sprite != nullptr");

    // Set our initialization function to perform any user initialization
    m_initFunc = initFunction;
}

/*! \brief 	Set a callback function which will be called
		each iteration of the main loop before drawing.
		@param void (*updateFunction)(App *) - the callback function
		@return void
*
*/
void App::UpdateCallback(void (*updateFunction)(App *)) {
    m_updateFunc = updateFunction;
}

/*! \brief Update the app's paintbrush function. Any method with the specified parameters and return
 * types could be used for painting in the display.
 * @param (*paintFunction)(App *, sf::Color, int size, int m_x, int m_y)) - the paint function
 * @return void
 *
 */
void App::UpdatePaintbrush(
        std::map<std::pair<int, int>, sf::Color> (*paintFunction)(App *, sf::Color, int size, int m_x, int m_y)) {
    m_paintFunc = paintFunction;
}

/*! \brief 	Set a callback function which will be called
		each iteration of the main loop after update.
		@param void (*drawFunction)(App *) - the draw callback
		@return void
*
*/
void App::DrawCallback(void (*drawFunction)(App *)) {
    m_drawFunc = drawFunction;
}

/*! \brief 	The main loop function which handles initialization
		and will be executed until the main window is closed.
		Within the loop function the update and draw callback
		functions will be called.
		@return void
*
*/
void App::Loop() {
    // Call the init function
    m_initFunc();

    // Start the main rendering loop
    while (m_window->isOpen()) {
        // Clear the window
        m_window->clear();
        // Updates specified by the user
        m_updateFunc(this);
        // Additional drawing specified by user
        m_drawFunc(this);
    }

}

/*! \brief 	Get color element in the App object.
 * @return int representing color
*
*/
int App::getColor() {
    return m_color.toInteger();
}

/*! \brief 	Delete this App object.
 * @return void
*
*/
App::~App() {};
/** 
 *  @file   App.hpp 
 *  @brief  App class interface
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-09-12
 ***********************************************/
#ifndef APP_HPP
#define APP_HPP

// Include our Third-Party SFML header
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
// Include standard library C++ libraries.
#include <deque>
#include <stack>
#include <iostream>
#include <list>
#include <iterator>
#include <stdlib.h>
// Project header files
#include "Command.hpp"
#include "UDPNetworkServer.hpp"
#include "UDPNetworkClient.hpp"

class App {
private:
// Member variables
    // Stack stores the next commands to redo.
    std::stack<std::list<Command *>> m_redo;
    // Stack that stores the last actions to occur.
    std::stack<std::list<Command *>> m_undo;
    /*!
     * sf::Image of the app
     */
    sf::Image *m_image;
    /*!
     * sf::Sprite of the app
     */
    sf::Sprite *m_sprite;
    /*!
     * sf::Texture of the app
     */
    sf::Texture *m_texture;
    /*!
     * Display window of the app, which renders the drawing
     */
    sf::RenderWindow *m_window;
    /*!
     * GUI window of the app, which contains buttons
     */
    sf::RenderWindow *m_gui;
    /*!
     * Display offset between the GUI button y-coordinates and the display window y-coordinates.
     * This data is necessary in order to prevent a user's click on a GUI button from resulting in a paint stroke
     * on the display window.
     */
    int m_displayOffset;

// Member functions
    // Store the address of our function pointer
    // for each of the callback functions.

    /*!
    * Initialization function pointer.
    */
    void (*m_initFunc)(void);

    /*!
     * Update function pointer.
     */
    void (*m_updateFunc)(App *);

    /*!
     * Draw function pointer, which is a pointer to a function that updates the texture.
     */
    void (*m_drawFunc)(App *);

public:
// Member Variables

    /*!
     * Mouse x-coordinate.
     */
    unsigned int mouseX;

    /*!
     * Mouse y-coordinate.
     */
    unsigned int mouseY;

    /*!
    * Current paintbrush size (radius)
    */
    int strokeSize;

    /*!
    * Whether this instance is running as a server or client. (not initialized in App constructor)
    */
    bool isServer;

    /*!
     * Size of paintbrush received in packet (not initialized in App constructor)
     */
    int receivedSize;

    /*!
     * Color of paintbrush received in packet (not initialized in App constructor)
     */
    sf::Color receivedColor;

    /*!
     * Paint function pointer, which is a pointer to a function that edits the sf::Image.
     */
    std::map<std::pair<int, int>, sf::Color> (*m_paintFunc)(App *, sf::Color color, int size, int m_x, int m_y);

    /*!
     * The server for our app. (not initialized in App constructor)
     */
    UDPNetworkServer *appServer;

    /*!
     * The client for our app. (not initialized in App constructor)
     */
    UDPNetworkClient *appClient;

    /*!
     * Canvas of the App.
     */
    sf::Color m_canvas;

    /*!
     * Current paint color.
     */
    sf::Color m_color;
    // Pixels that the user has painted in the current brush stroke
    std::list<Command *> m_paintedPixels;

// Member functions
    // Constructor
    App();

    // Add a command to stack
    void AddCommand();

    // Undo a command
    void UndoCommand();

    // Redo a command
    void RedoCommand();

    // Get app image
    sf::Image &GetImage();

    // Get app texture
    sf::Texture &GetTexture();

    // Get app GUI render window
    sf::RenderWindow &GetGui();

    // Get app sprite
    sf::Sprite &GetSprite();

    // Get offset: pixel value for y-coordinate offset of GUI buttons vs. canvas
    int GetDisplayOffset();

    // Get display window
    sf::RenderWindow &GetDisplayWindow();

    // Get dimensions of window
    sf::Vector2u GetDisplayDimensions();

    // Destroy the app
    void Destroy();

    // Initialize app
    void Init(void (*initFunction)(void));

    // Execute individual pixel command
    void ExecuteCommand(Command *command);

    // Fill display with one color command
    void FillDisplay(Command *command);

    // Update callback function
    void UpdateCallback(void (*updateFunction)(App *));

    // Draw function
    void DrawCallback(void (*drawFunction)(App *));

    // Update paintbrush function
    void UpdatePaintbrush(
            std::map<std::pair<int, int>, sf::Color> (*paintFunction)(App *, sf::Color, int size, int m_x, int m_y));

    // Main app loop
    void Loop();

    // Get app current color
    int getColor();

    // Destructor for app
    virtual ~App();

};


#endif
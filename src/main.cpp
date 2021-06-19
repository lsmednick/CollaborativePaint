/**
 *  @file   main.cpp
 *  @brief  Entry point into the program.
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/

// Include our Third-Party SFML header
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Network.hpp>
// Include standard library C++ libraries.
#include <iostream>
#include <map>
#include <typeinfo>
#include <stdlib.h>
// Project header files
#include "App.hpp"
#include "Command.hpp"
#include "Draw.hpp"
#include "FillDisplay.hpp"
#include "Packet.hpp"
#include "UDPNetworkServer.hpp"
#include "UDPNetworkClient.hpp"

#define GL_SILENCE_DEPRECATION // Necessary to silence GL deprecation warnings
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>

// NUKLEAR definitions - for our GUI
#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_SFML_GL2_IMPLEMENTATION

#include "nuklear.h"
#include "nuklear_sfml_gl2.h"

/*! \brief 	Initialization function for the app.
 * This function simply outputs "Starting the App" in the console.
 *
 * @return void
*
*/
void initialization(void){
    std::cout << "Starting the App" << std::endl;
}

/*! \brief 	Send package to either the client or to the server.
 * @param minipaint the App that the network is working upon
 * @param p the Packet to be sent
 * @return void
*
*/
void packetSender(App* minipaint, myPacket p) {
    if (minipaint->isServer) {
        minipaint->appServer->send(p);
    } else {
        minipaint->appClient->sendCommand(p);
    }
}

/*!
 * /brief The drawLayout function processes user interactions with buttons in the GUI.
 * It updates the paintbrush color and size, undoes/redoes an action,
 * or performs a fill command depending on which button the user clicks.
 * @param minipaint the App that the network is working upon
 * @param bg the stored background color settings of the GUI
 * @param ctx context for Nuklear
 * @return Packet storing command
 */
myPacket drawLayout(App *minipaint, struct nk_context *ctx, struct nk_colorf &bg) {
    myPacket p;
    int command;

    if (nk_begin(ctx, "Minipaint App", nk_rect(0, 0, 1000, 150),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                 NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
        /* fixed widget pixel width for undo or redo command */
        nk_layout_row_static(ctx, 30, 80, 2);
        // Create packet for undo command
        if (nk_button_label(ctx, "undo")) {
            command = 3;
            p << command << 0 << 0 << 0 << 0;
            packetSender(minipaint, p);
        }
        // Create packet for redo command
        if (nk_button_label(ctx, "redo")) {
            command = 4;
            p << command << 0 << 0 << 0 << 0;
            packetSender(minipaint, p);
        }

        /* fixed widget pixel width for setting brush size */
        nk_layout_row_dynamic(ctx, 30, 6);
        if (nk_button_label(ctx, "Small brush")) {
            minipaint->strokeSize = 2;
        }
        if (nk_button_label(ctx, "Medium brush")) {
            minipaint->strokeSize = 4;
        }

        if (nk_button_label(ctx, "Large brush")) {
            minipaint->strokeSize = 6;
        }
        if (nk_button_label(ctx, "-")) {
            if (minipaint->strokeSize > 1) {
                minipaint->strokeSize--;
            }
        }
        if (nk_button_label(ctx, "+")) {
            minipaint->strokeSize++;
        }
        // Send packet for fill command
        if (nk_button_label(ctx, "fill")) {
            command = 5;
            p << command << 0 << 0 << minipaint->getColor() << 0;
            packetSender(minipaint, p);
            minipaint->GetTexture().loadFromImage(minipaint->GetImage());
        }

        /* fixed widget window ratio width */
        nk_layout_row_dynamic(ctx, 30, 6);
        if (nk_button_label(ctx, "black")) {
            minipaint->m_color = sf::Color::Black;
        }
        if (nk_button_label(ctx, "white")) {
            minipaint->m_color = sf::Color::White;
        }
        if (nk_button_label(ctx, "red")) {
            minipaint->m_color = sf::Color::Red;
        }
        if (nk_button_label(ctx, "green")) {
            minipaint->m_color = sf::Color::Green;
        }
        if (nk_button_label(ctx, "blue")) {
            minipaint->m_color = sf::Color::Blue;
        }
        if (nk_button_label(ctx, "eraser")) {
            minipaint->m_color = minipaint->m_canvas;
        }

    }
    nk_end(ctx);
    minipaint->GetTexture().loadFromImage(minipaint->GetImage());
    minipaint->GetSprite().setTexture(minipaint->GetTexture());

    return p;
}

/*!
 * \brief The paint function takes an App and a color, and paints the app's window with the specified color.
 * The paint function is sensitive to the app's paintbrush size and will paint neighboring pixels of the mouse
 * position, so that the proper diameter of brushstroke is applied.
 * @param minipaint the App to paint upon
 * @param color the color to be painted with
 * @param radius the radius of the brushstroke
 * @param m_x the x-value of the central pixel for this paint action
 * @param m_y the y-value of the central pixel for this paint action
 * @return std::map<std::pair<int, int>, sf::Color> a map of the affected pixel colors prior to paint action
 *
 */
std::map<std::pair<int, int>, sf::Color> paint(App *minipaint, sf::Color color, int radius, int m_x, int m_y) {
    std::map<std::pair<int, int>, sf::Color> priorPixelValues;
    for (int i = m_x - radius; i < m_x + radius; i++) {
        for (int j = m_y - radius; j < m_y + radius; j++) {
            if (minipaint->GetSprite().getGlobalBounds().contains(i, j)) {
                priorPixelValues[std::make_pair(i, j)] = minipaint->GetImage().getPixel(i, j);
                minipaint->GetImage().setPixel(i, j, color);
            }
        }
    }
    return priorPixelValues;
}

/*!
 * \brief Similar to the paint function, this will allow for each participant to receive information to be drawn.
 * @param minipaint the App to paint upon
 * @param color the color to draw with
 * @param m_x the central pixel of the draw action
 * @param m_y the central pixel of the draw action
 * @return std::map<std::pair<int, int>, sf::Color> map of the affected pixels' colors prior to this action
 *
 */
std::map<std::pair<int, int>, sf::Color> receivePaint(App* minipaint, sf::Color color, int m_x, int m_y) {
    std::map<std::pair<int, int>, sf::Color> priorPixelValues;
    for (int i = m_x - minipaint->receivedSize; i < m_x + minipaint->receivedSize; i++) {
        for (int j = m_y -minipaint->receivedSize; j < m_y + minipaint->receivedSize; j++) {
            if (minipaint->GetSprite().getGlobalBounds().contains(i, j)) {
                minipaint->GetImage().setPixel(i, j, color);
            }
        }
    }
    return priorPixelValues;
}

/*!
 * \brief The keyEvent method is a helper method to the update() main method.
 * It interprets events related to the keyboard, such as a user
 * pressing a specific keyboard button, and processes these user actions in the App program.
 * @param event the event to interpret
 * @param minipaint the App to update
 * @return myPacket object - a packet storing command information
 */
myPacket keyEvent(sf::Event event, App* minipaint) {
    myPacket p;
    int command;
    if (event.key.code == sf::Keyboard::Z) {
        // send this command
        command = 3;
        p << command << 0 << 0 << 0 << 0;
        packetSender(minipaint, p);
    }
    else if (event.key.code == sf::Keyboard::Y) {
        command = 4;
        p << command << 0 << 0 << 0 << 0;
        packetSender(minipaint, p);
    }

    else if (event.key.code == sf::Keyboard::Space) {
        command = 5;
        p << command << 0 << 0 << minipaint->getColor() << 0;
        packetSender(minipaint, p);
    }

    else if(event.key.code == sf::Keyboard::Escape) {
        command = 6;
        p << command << 0 << 0 << 0 << 0;
        packetSender(minipaint, p);
        minipaint->GetGui().close();
        exit(EXIT_SUCCESS);
    }


    switch(event.key.code) {
        case sf::Keyboard::Num1 :
            minipaint->m_color = sf::Color::Black;
            break;
        case sf::Keyboard::Num2 :
            minipaint->m_color = sf::Color::White;
            break;
        case sf::Keyboard::Num3 :
            minipaint->m_color = sf::Color::Red;
            break;
        case sf::Keyboard::Num4 :
            minipaint->m_color = sf::Color::Green;
            break;
        case sf::Keyboard::Num5 :
            minipaint->m_color = sf::Color::Blue;
            break;
        case sf::Keyboard::Num6 :
            minipaint->m_color = sf::Color::Yellow;
            break;
        case sf::Keyboard::Num7 :
            minipaint->m_color = sf::Color::Magenta;
            break;
        case sf::Keyboard::Num8 :
            minipaint->m_color = sf::Color::Cyan;
            break;
        case sf::Keyboard::B:
            minipaint->strokeSize = 5;
            break;
        case sf::Keyboard::M:
            minipaint->strokeSize = 3;
            break;
        case sf::Keyboard::S:
            minipaint->strokeSize = 1;
            break;
        case sf::Keyboard::RBracket:
            if (minipaint->strokeSize < 5) {
                minipaint->strokeSize++;
            }
            break;
        case sf::Keyboard::LBracket:
            if (minipaint->strokeSize > 1) {
                minipaint->strokeSize--;
            }
            break;
        case sf::Keyboard::E:
            minipaint->m_color = minipaint->m_canvas;
            break;
        default :
            break;
    }

    return p;
}

/*!
 * /brief The mouseEvent is a helper method for the main update() function.
 * This method processes user actions when the user is holding down the left mouse button.
 * @param minipaint the App to update
 * @param event the event to interpret
 * @return myPacket object - a packet storing command information
 */
myPacket mouseEvent(sf::Event event, App* minipaint) {
    myPacket p;
    int command;
    sf::Vector2i mousePosition = sf::Mouse::getPosition(minipaint->GetGui());
    //Check for duplicates to prevent waste from idle
    bool rpts = (minipaint->mouseX != mousePosition.x || minipaint->mouseY != mousePosition.y);
    //Check if within window to prevent segfault
    bool inBounds = minipaint->GetSprite().getGlobalBounds().contains(mousePosition.x,
                                                                      mousePosition.y -
                                                                      minipaint->GetDisplayOffset());
    //Check both this is not a duplicate pixel recording and the pixel's coordinate is in-bounds
    if (rpts && inBounds) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            minipaint->mouseX = mousePosition.x;
            minipaint->mouseY = mousePosition.y - minipaint->GetDisplayOffset();
            int color = minipaint->getColor();
            command = 1;
            p << command << minipaint->mouseX << minipaint->mouseY << color << minipaint->strokeSize;
            packetSender(minipaint, p);
            minipaint->GetTexture().loadFromImage(minipaint->GetImage());
        }


        else if (event.type == sf::Event::MouseButtonReleased) {

            std::cout << "Mouse released" << std::endl;
            if (!minipaint->m_paintedPixels.empty()) {
                command = 2;
                p << command << 0 << 0 << 0 << 0;
                packetSender(minipaint, p);
            }
        }
    }

    return p;
}

/*!
 * packetHandler function translates myPacket objects into actions done upon the App via networking.
 * @param minipaint the App to act upon
 * @param p the myPacket object storing command information
 * @return void
 */
void packetHandler(App* minipaint, myPacket p) {
    int command;
    int x;
    int y;
    int color;
    int size;
    p >> command >> x >> y >> color >> size;
    if (command == 1) {
        sf::Color p_color = sf::Color(color);
        Draw *pixel = new Draw(minipaint, x, y, p_color, size);
        minipaint->ExecuteCommand(pixel);
    }
    else if (command == 2) {
        std::cout << "released mouse button" << std::endl;
        if (!minipaint->m_paintedPixels.empty()) {
            minipaint->AddCommand();
            //Reload for the next stroke
            minipaint->m_paintedPixels = std::list<Command *>();
        }
    }
    else if (command == 3) {
        std::cout << "undo" << std::endl;
        minipaint->UndoCommand();
    }
    else if(command == 4) {
        std::cout << "redo" << std::endl;
        minipaint->RedoCommand();
    }
    else if(command == 5) {
        std::cout << "fill screen" << std::endl;
        minipaint->FillDisplay(new FillDisplay(minipaint, color));
        minipaint->GetTexture().loadFromImage(minipaint->GetImage());
    }


}

/*!
 * \brief The updateDisplay action refreshes the GUI and display windows with updated sprites and
 * Nuklear GUI representations.
 * @param minipaint the App to act upon
 * @param &bg the background color information for the GUI
 * @return void
 */
void updateDisplay(App *minipaint, struct nk_colorf &bg) {
    // Perform GUI window updates
    minipaint->GetGui().setActive(true);
    minipaint->GetGui().clear();
    // Clear color
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT);
    nk_sfml_render(NK_ANTI_ALIASING_ON);
    minipaint->GetGui().draw(minipaint->GetSprite());
    minipaint->GetGui().display();

    // Perform display window updates
    minipaint->GetDisplayWindow().setActive(true);
    minipaint->GetDisplayWindow().clear();
    // Draw our sprite
    minipaint->GetDisplayWindow().draw(minipaint->GetSprite());
    minipaint->GetDisplayWindow().display();
}

/*!
 * \brief The update action receives information from participants and refreshes the GUI and display windows
 * with updated sprites and Nuklear GUI representations.
 * @param minipaint the App to act upon
 * @return void
 */
void update(App* minipaint) {

    int command;
    int x;
    int y;
    int color;
    int size;
    myPacket r;

    myPacket p;

    glViewport(0, 0, minipaint->GetGui().getSize().x, minipaint->GetGui().getSize().y);

    // Create Nuklear context
    nk_context *ctx = nk_sfml_init(&minipaint->GetGui());

    // Load Fonts: if none of these are loaded a default font will be used
    struct nk_font_atlas *atlas;
    nk_sfml_font_stash_begin(&atlas);
    nk_sfml_font_stash_end();

    // Initialize Nuklear bg struct for colors
    struct nk_colorf bg;
    bg.r = 255.00f, bg.g = 255.00f, bg.b = 255.00f, bg.a = 1.0f;

    sf::Event event;

    while (minipaint->GetDisplayWindow().isOpen() && minipaint->GetGui().isOpen()) {

        if (minipaint->isServer) {
            r = minipaint->appServer->listener();
        } else {
            r = minipaint->appClient->receiveData();
        }

        packetHandler(minipaint, r);
        r >> command >> x >> y >> color >> size;
        if (command == 1) {
            sf::Color r_color = sf::Color(color);
            minipaint->receivedSize = size;
            minipaint->receivedColor = r_color;
            receivePaint(minipaint, r_color, x, y);
        }
        // Poll the display window for the user closing the window
        while (minipaint->GetDisplayWindow().pollEvent(event)) {

            // Handle closing the display window
            if (event.type == sf::Event::Closed) {
                minipaint->GetDisplayWindow().close();
                minipaint->GetGui().close();
                exit(EXIT_SUCCESS);
            }
        }

        // Capture input from the nuklear GUI
        nk_input_begin(ctx);
        while (minipaint->GetGui().pollEvent(event)) {

            // Check if user is pressing the left mouse button
            // This will record a single click or burst
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || event.type == sf::Event::MouseButtonReleased) {
                p = mouseEvent(event, minipaint);
                packetHandler(minipaint, p);

            } else if (event.type == sf::Event::KeyReleased) {
                p = keyEvent(event, minipaint);
                packetHandler(minipaint, p);

            }
            // Handle GUI button-linked events: Changing color, fill screen, undo/redo
            nk_sfml_handle_event(&event);

        }

        if (event.type == sf::Event::Closed) {
            minipaint->GetDisplayWindow().close();
            minipaint->GetGui().close();
            exit(EXIT_SUCCESS);
        }


        while (minipaint->GetDisplayWindow().pollEvent(event)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || event.type == sf::Event::MouseButtonReleased) {
                p = mouseEvent(event, minipaint);
            } else if (event.type == sf::Event::KeyReleased) {
                p = keyEvent(event, minipaint);
            }
            std::cout << "handling packet for display window" << std::endl;
            packetHandler(minipaint, p);
        }

        // Complete input from nuklear GUI
        nk_input_end(ctx);

        // Implement GUI commands
        p = drawLayout(minipaint, ctx, bg);
        packetHandler(minipaint, p);

        // Update the display
        updateDisplay(minipaint, bg);
    }

}

/*! \brief 	The draw call refreshes the texture of the minipaint app.
* @param minipaint the App object to refresh
 * @return void
*/
void draw(App* minipaint){
    // Static variable
    static int refreshRate = 0;
    ++refreshRate;
    if (refreshRate > 10) {
        minipaint->GetTexture().loadFromImage(minipaint->GetImage());
        refreshRate = 0;
    }

}

/*! \brief 	Run the app
 * @param minipaint the App object to run
 * @return void
*
*/
void runApp(App* minipaint){
    // Setup the update function
    minipaint->UpdateCallback(&update);

    // Set up window and canvas components
    minipaint->Init(&initialization);
    // Setup the Draw Function for reloading screen per refresh rate
    minipaint->DrawCallback(&draw);
    // Set up the initial paintbrush function
    minipaint->UpdatePaintbrush(&paint);
    // Call the main loop function
    minipaint->Loop();
    // Destroy our app
    minipaint->Destroy();
    // Shutdown nk_sfml
    nk_sfml_shutdown();

}

/*! \brief 	Run the server
* @param minipaint the App that the server should be delegated to
 * @return void
*/
void runServer(App* minipaint){
    unsigned short sport;
    minipaint->isServer = true;
    std::string uname;
    std::cout << "Initializing the server..." << std::endl;
    std::cout << "Enter your username pls: ";
    std::cin >> uname;
    std::cout << "Enter your port number: ";
    std::cin >> sport;
    UDPNetworkServer* server = new UDPNetworkServer("Server Name", sf::IpAddress::getLocalAddress(), 50001);
    minipaint->appServer = server;
    minipaint->appServer->setUsername(uname);
    minipaint->appServer->start();
}

/*! \brief 	Run the client
* @param minipaint the App that the client should join
 * @return void
*/
void runClient(App* minipaint){
    std::string uname;
    unsigned short cport;
    minipaint->isServer = false;
    std::cout << "Enter your username pls: ";
    std::cin >> uname;
    std::cout << "Enter your port number: ";
    std::cin >> cport;

    UDPNetworkClient* cli = new UDPNetworkClient(uname, cport);
    minipaint->appClient = cli;
    minipaint->appClient->joinServer(sf::IpAddress::getLocalAddress(), 50001);
    minipaint->appClient->setUsername(uname);
}

/*! \brief 	The entry point into our program.
*
*/
int main(){
    std::string role;
    std::cout << "Enter (s) for Server, Enter (c) for client: " << std::endl;
    std::cin >> role;
    App* minipaint = new App();
    if (role[0] == 's') {
        runServer(minipaint);
        runApp(minipaint);

    } else if (role[0] == 'c') {
        runClient(minipaint);
        runApp(minipaint);
    }

    return 0;
}
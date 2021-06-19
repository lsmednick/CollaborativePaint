/**
 *  @file   main_test.cpp
 *  @brief  Unit Tests for our program
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-09-12
 ***********************************************/

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

// Include our Third-Party SFML header
#include <SFML/Graphics/Sprite.hpp>
// Include standard library C++ libraries.
#include <iostream>
#include <string>
// Project header files
#include "App.hpp"
#include "Command.hpp"
#include "Draw.hpp"
#include "FillDisplay.hpp"
#include "Packet.hpp"
#include "UDPNetworkServer.hpp"
#include "UDPNetworkClient.hpp"

// Setup for tests: Define initialization function
void initialization() {
    std::cout << "Starting the App" << std::endl;
}

// Setup for tests: Define standard paint function
std::map<std::pair<int, int>, sf::Color> standardPaintFunc(App *minipaint, sf::Color color,
                                                           int radius, int m_x, int m_y) {
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

// Setup for tests: Define new (empty) paint function to test that minipaint can switch brushes
std::map<std::pair<int, int>, sf::Color> newPaintFunc(App *minipaint,
                                                      sf::Color color, int radius, int m_x, int m_y) {
    return std::map<std::pair<int, int>, sf::Color>();
}

/*! \brief 	Basic test to initialize and destroy the program
*
*/
TEST_CASE("Basic init and destroy test") {
    App *minipaint = new App();
    minipaint->Init(&initialization);
    // Destroy our app
    minipaint->Destroy();
}



/*! \brief 	Test that the app can change its paintbrush function. (Test case for new feature)
*
*/
TEST_CASE("Change app paintbrush") {
    App *minipaint = new App();
    minipaint->Init(&initialization);
    minipaint->UpdatePaintbrush(&newPaintFunc);
    REQUIRE(minipaint->m_paintFunc == newPaintFunc);
    // Destroy our app
    minipaint->Destroy();
}

/*! \brief 	Test that changing the paintbrush diameter ("size") changes the number of pixels
 * that are drawn upon in one Draw command. (Test case for new feature)
*
*/
TEST_CASE("Change app paintbrush diameter affects correct pixels - test three diameters") {
    App *minipaint = new App();
    minipaint->Init(&initialization);
    // Draw at this pixel
    minipaint->mouseX = 150;
    minipaint->mouseY = 200;

    minipaint->UpdatePaintbrush(&standardPaintFunc);

    int diameters[] = {2, 4, 6};

    for (int diameter : diameters) {
        minipaint->strokeSize = diameter;
        Draw *pixel = new Draw(minipaint);
        minipaint->ExecuteCommand(pixel);
        for (int j = minipaint->mouseX - minipaint->strokeSize; j < minipaint->mouseX + minipaint->strokeSize; j++) {
            for (int k = minipaint->mouseY - minipaint->strokeSize;
                 k < minipaint->mouseY + minipaint->strokeSize; k++) {
                REQUIRE(minipaint->GetImage().getPixel(j, k) == sf::Color::Black);
            }
        }
    }
    // Destroy our app
    minipaint->Destroy();
}



/*! \brief Test that the App differentiates different brush strokes by storing the affected pixels of one
 * stroke as a single stack in undo
 */
TEST_CASE("App differentiates different brush strokes by adding one stroke as a single stack to undo list") {
    App *minipaint = new App();
    minipaint->Init(&initialization);
    minipaint->mouseX = 150;
    minipaint->mouseY = 200;
    minipaint->m_color = sf::Color::Red;
    minipaint->strokeSize = 1;

    minipaint->UpdatePaintbrush(&standardPaintFunc);

    int x_pos[] = {200, 400, 350};
    int y_pos[] = {100, 200, 300};

    for (int i = 0; i < 3; i++) {
        minipaint->mouseX = x_pos[i];
        minipaint->mouseY = y_pos[i];
        Draw *pixel = new Draw(minipaint);
        minipaint->ExecuteCommand(pixel);
    }

    // Ensure all draw commands were added to stack of painted pixels (within one brush stroke)
    REQUIRE(minipaint->m_paintedPixels.size() == 3);

    // Empty the current brush stroke stack by calling "AddCommand" which adds current stroke to undo list
    minipaint->AddCommand();
    minipaint->m_paintedPixels = std::list<Command *>();

    // Run an "undo"
    minipaint->UndoCommand();

    // Ensure the stack of painted pixels for the full brush stroke is empty
    for (int i = 0; i < 3; i++) {
        REQUIRE(minipaint->GetImage().getPixel(x_pos[i], y_pos[i]) == sf::Color::White);
    }
}


/*! \brief 	Test that undoing a "fill screen" operation restores the prior canvas values
*
*/
TEST_CASE("test undoing a fill color operation") {
    App *minipaint = new App();
    minipaint->Init(&initialization);
    minipaint->mouseX = 150;
    minipaint->mouseY = 200;
    minipaint->m_color = sf::Color::Red;
    minipaint->strokeSize = 1;

    minipaint->UpdatePaintbrush(&standardPaintFunc);

    //draw red pixel on white canvas, change to blue canvas, undo draw pixel
    Draw *pixel = new Draw(minipaint);
    minipaint->ExecuteCommand(pixel);

    minipaint->m_color = sf::Color::Blue;
    minipaint->FillDisplay(new FillDisplay(minipaint, minipaint->getColor()));
    minipaint->GetTexture().loadFromImage(minipaint->GetImage());

    //undo pixel on blue canvas, pixel should be red
    minipaint->UndoCommand();
    REQUIRE(minipaint->GetImage().getPixel(150, 200) == sf::Color::Red);
    // Pixel not drawn upon before the "fill" command should be white
    REQUIRE(minipaint->GetImage().getPixel(200, 200) == sf::Color::White);

    // Destroy our app
    minipaint->Destroy();
}

/*! \brief 	Test the application can correctly translate the "Offset" attribute,
 * which indicates the offset from the GUI buttons to the window in which users can draw, into
 * judgments about whether a pixel is in bounds for drawing.
*
*/
TEST_CASE("test mouse behaviors inside the GUI button coordinates don't result in draw actions") {
    App *minipaint = new App();
    minipaint->Init(&initialization);
    minipaint->mouseX = 400;
    minipaint->mouseY = 10; // If the Y value of the GUI is 10, then the coordinate is not in-bounds
    minipaint->m_color = sf::Color::Red;
    minipaint->strokeSize = 1;
    minipaint->UpdatePaintbrush(&standardPaintFunc);
    bool inBounds = minipaint->GetSprite().getGlobalBounds().contains(minipaint->mouseX,
                                                                      minipaint->mouseY -
                                                                      minipaint->GetDisplayOffset());
    REQUIRE(!inBounds);
    minipaint->Destroy();
}

/*! \brief 	Test client joining server and sending packet to server
*
*/
TEST_CASE("Test client joining server") {
    UDPNetworkServer* server = new UDPNetworkServer("testServer", sf::IpAddress::getLocalAddress(), 50001);
    UDPNetworkClient* client1 = new UDPNetworkClient("testClient", 55000);
    int joined = client1->joinServer(sf::IpAddress::getLocalAddress(), 50001);
    REQUIRE(joined == 0);
    server->~UDPNetworkServer();
    client1->~UDPNetworkClient();
}

/*! \brief 	Test two clients joining server and sending packet to server
*
*/
TEST_CASE("Test client joining server and sending packet") {
    myPacket p;
    p << 0 << 0 << 0 << 0 << 0;
    UDPNetworkServer* server = new UDPNetworkServer("testServer", sf::IpAddress::getLocalAddress(), 50001);
    UDPNetworkClient* client1 =  new UDPNetworkClient("testClient", 55000);
    client1->joinServer(sf::IpAddress::getLocalAddress(), 50001);
    int send = client1->sendCommand(p);
    REQUIRE(send == 0);
server->~UDPNetworkServer();
client1->~UDPNetworkClient();
}

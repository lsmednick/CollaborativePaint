/**
 *  @file   UDPNetworkClient.hpp
 *  @brief  Represents a UDP network client for the app.
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/

#ifndef UDP_NETWORK_CLIENT_HPP
#define UDP_NETWORK_CLIENT_HPP
// Include our Third-Party SFML header
#include <SFML/Network.hpp>
#include <SFML/Graphics/Color.hpp>
// Project header files
#include "Command.hpp"
#include "Packet.hpp"
// Include standard library C++ libraries.
#include <string>
#include <vector>

class UDPNetworkClient {
public:
    // Default constructor
    UDPNetworkClient();

    // Second constructor with username and port input
    UDPNetworkClient(std::string username, unsigned short port);

    // Default destructor
    virtual ~UDPNetworkClient();

    // Member function to join server
    int joinServer(sf::IpAddress serverIp, unsigned short serverPort);

    // Member function to send data to server
    int sendCommand(myPacket p);

    // Member function to send string to server
    int sendString(std::string str);

    // Member function to receive data from the server
    myPacket receiveData();

    // Setter for the client username
    int setUsername(std::string new_name);

    // Getter for the client username
    std::string getUsername();

    // Handler for when the client leaves
    void handleClientLeaving();

private:
    // Username of the client
    std::string username;
    // The port which we will try to communicate from
    unsigned short m_port{};
    // The server port which we will send information to
    unsigned short serverPort{};
    // The server ip which we will send information to
    sf::IpAddress serverIpAddress;
    // A UDP socket for our client to create an end-to-end communcation
    // with another machine
    sf::UdpSocket socket;
};

#endif

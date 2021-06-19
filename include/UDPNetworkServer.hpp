/**
 *  @file   UDPNetworkServer.hpp
 *  @brief  Represents a UDP network server for the app.
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/

#ifndef UDP_NETWORK_SERVER_HPP
#define UDP_NETWORK_SERVER_HPP

// Include our Third-Party SFML header
#include <SFML/Network.hpp>
// Project header files
#include "Command.hpp"
#include "Packet.hpp"
// Include standard library C++ libraries.
#include <string>
#include <vector>


// Class representing a non-blocking UDP server
class UDPNetworkServer {
public:
    // Default constructor
    UDPNetworkServer();

    // Other constructor
    UDPNetworkServer(std::string n, sf::IpAddress ip, unsigned short port);

    // Default destructor
    virtual ~UDPNetworkServer();

    // Member function to start the server
    int start();

    // Member function to stop the server and remove clients
    int stop();

    // Packet listener
    myPacket listener();

    // Member function to send packet
    int send(myPacket p);

    // Member function for client leaving
    int clientLeaving();

    // Member function to set username
    int setUsername(std::string new_name);

private:
    // Name for the server
    std::string name;

    // Handles when client joins the server
    int clientJoining(unsigned short clientPort, sf::IpAddress clientIp);

    // Flag signaling if the server should stop
    bool flag;

    // Ip for our UDP server
    sf::IpAddress serverIp;

    // The port number
    unsigned short m_port;

    // UDP socket for our server
    sf::UdpSocket sock;

    // Capture status of server
    bool m_status;

    // DATA STRUCTURES
    // Map to hold all of the clients
    std::map<unsigned short, sf::IpAddress> activeClients;
    // Vector to hold all packets
    std::vector<std::string> packetHistory;
};

#endif

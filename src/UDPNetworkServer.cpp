/**
 *  @file   UDPNetworkServer.cpp
 *  @brief  Entry point into the program.
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/

#include "UDPNetworkServer.hpp"

#include <SFML/Network.hpp>
#include <iostream>

/*!
 * Constructor for a UDPNetwork server, with no parameters.
 */UDPNetworkServer::UDPNetworkServer() {
    std::cout << "Default Constructor" << std::endl;
}

/*!
 * Second constructor for a UDPNetwork server, with parameters for name, IP address, and port.
 * @param name the username
 * @param address the IP address for the server
 * @param port the port for server
 */UDPNetworkServer::UDPNetworkServer(std::string n, sf::IpAddress address, unsigned short port) {
    name = n;
    serverIp = address;
    m_port = port;
    std::cout << "Server Constructor" << std::endl;
}

/*!
 * Default destructor for UDPNetworkServer
 */
UDPNetworkServer::~UDPNetworkServer() {
    std::cout << "Server Destructor" << std::endl;
}

/*!
 * Method to start the server
 * @return int representing success of operation (0 = success)
 */
int UDPNetworkServer::start() {
    std::cout << "Starting UDP Network Server" << std::endl;
    int status;
    if (status == (sock.bind(m_port) != sf::Socket::Done)) {
        std::cout << "Error! Unable to bind. " << status << std::endl;
        return status;
    }
    m_status = true;
    sock.setBlocking(false);
    return 0;
}

/*!
 * Method to listen for client connections
 * @return myPacket data packet
 */
myPacket UDPNetworkServer::listener() {
    // Set our boolean flag to true and create non-blocking
    // UDP server

    // Main server loop
    myPacket in;
    sf::IpAddress senderIp;
    unsigned short senderPort;
    if (sock.receive(in, senderIp, senderPort) == sf::Socket::Done) {
        std::cout << "From Client: " << std::endl;
        flag = true;
        std::map<unsigned short, sf::IpAddress>::iterator clientIter;
        clientIter = activeClients.find(senderPort);
        if (clientIter == activeClients.end()) {
            std::cout << "First time joiner!" << std::endl;
            clientJoining(senderPort, senderIp);
            activeClients[senderPort] = senderIp;
        }

        std::map<unsigned short, sf::IpAddress>::iterator ipIter;
        for (ipIter = activeClients.begin(); ipIter != activeClients.end(); ipIter++) {
            if (senderPort != ipIter->first) {
                std::cout << "recieved and sending data to" << ipIter->first << std::endl;
                sock.send(in, ipIter->second, ipIter->first);
            }
        }
    }
    return in;
}

/*!
 * Method to send a packet to a target IP
 * @param p the packet to send
 * @return an int representing success of the operation (success = 0)
 */
int UDPNetworkServer::send(myPacket p) {
    unsigned short senderPort;
    std::map<unsigned short, sf::IpAddress>::iterator ipIter;
    for (ipIter = activeClients.begin(); ipIter != activeClients.end(); ipIter++) {
        std::cout << "sending data to" << ipIter->first << std::endl;
        sock.send(p, ipIter->second, ipIter->first);
    }
    return 0;
}

/*!
 * Method to handle a client joining the server
 * @param clientPort the client's port
 * @param clientIp the client's IP address
 * @return an int representing success of the operation (success = 0)
 */
int UDPNetworkServer::clientJoining(unsigned short clientPort, sf::IpAddress clientIp) {
    std::cout << "Updating new client" << std::endl;
    myPacket p;
    p << 0 << 0 << 0 << 0 << 0;
    if (sock.send(p, clientIp, clientPort) != sf::Socket::Done) {
        std::cout << "Could not update new client" << std::endl;
        return 1;
    } else {
        std::cout << "Successfully updated new client!" << std::endl;
    }
    return 0;
}

/*!
 * Method to stop the server
 * @return an int representing success of the operation (success = 0)
 */
int UDPNetworkServer::stop() {
    flag = false;
    return 0;
}


/*!
 * Method to indicate a client has left
 * @return an int representing success of the operation (success = 0)
 */
int UDPNetworkServer::clientLeaving() {
    std::cout << "Client has left!" << std::endl;
    return 0;
}

/*!
 * Method to set server username
 * @param new_name the new username
 * @return an int representing success of the operation (success = 0)
 */
int UDPNetworkServer::setUsername(std::string new_name) {
    name = new_name;
    return 0;
}
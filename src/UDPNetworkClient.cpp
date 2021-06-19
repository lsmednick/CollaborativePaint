/**
 *  @file   UDPNetworkClient.cpp
 *  @brief  Entry point into the program.
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/

#include <SFML/Network.hpp>
#include "UDPNetworkClient.hpp"
#include <iostream>


/*!
 * Constructor for a UDPNetwork client, with no parameters.
 */
UDPNetworkClient::UDPNetworkClient() {
    std::cout << "Default constructor" << std::endl;
}

/*!
 * Second constructor for a UDPNetwork client, with parameters for name and port.
 * @param name the username
 * @param port the port for socket connection
 */
 UDPNetworkClient::UDPNetworkClient(std::string name, unsigned short port) {
    // Assign username and port to private member variables
    username = name;
    m_port = port;
    // Set up socket for UDP connection
    socket.bind(m_port);
    // Set socket to be non-blocking
    socket.setBlocking(false);
}

/*!
 * Default destructor for UDPNetworkClient
 */
UDPNetworkClient::~UDPNetworkClient() {}


/*!
 * Method for UDPNetworkClient to join a server
 * @param ip the IpAddress for joining server
 * @param servPort the server port with which the connection should be attempted
 * @return int representing success of join (0 = success)
 */
int UDPNetworkClient::joinServer(sf::IpAddress ip, unsigned short servPort) {
    myPacket p;
    std::cout << "UDPClient will attempt to join server..." << std::endl;
    p << 0 << 0 << 0 << 0 << 0;
    serverIpAddress = ip;
    serverPort = servPort;
    if (socket.send(p, ip, serverPort) != sf::Socket::Done) {
        std::cout << "Could not join server" << std::endl;
        return 1;
    } else {
        std::cout << "Successfully joined!" << std::endl;
    }
    return 0;
}

/*!
 * Method to send command data from UDPNetworkClient to server
 * @param p the packet command to be sent to server
 * @return int representing success of sending command (0 = success)
 */
int UDPNetworkClient::sendCommand(myPacket p) {
    try {
        if (socket.send(p, serverIpAddress, serverPort) != sf::Socket::Done) {
            std::cout << "Client error? Wrong IP?" << std::endl;
            return 1;
        } else {
            std::cout << "Client (" << username << ") sending packet" << std::endl;
        }
    }
    catch (const std::exception &e) {
        std::cout << "Exception has been caught!" << std::endl;
    }
    return 0;
}

/*!
 * Method to send a string from UDPNetworkClient to server
 * @param s the string to be sent to the server
 * @return int representing success of sending string (0 = success)
 */
int UDPNetworkClient::sendString(std::string s) {
    if (s.length() <= 0) {
        return -1;
    }
    s += " (from " + username + ")";
    if (socket.send(s.c_str(), s.length() + 1, serverIpAddress, serverPort) == sf::Socket::Done) {
        std::cout << "Client (" << username << ") sending string" << std::endl;
    }
    return 0;
}

/*!
 * Method for UDPNetworkClient to receive data from server
 * @return a packet, storing the data received from server
 */
myPacket UDPNetworkClient::receiveData() {
    myPacket in;
    std::string size;
    std::string command = " ";
    sf::IpAddress copyAddress = sf::IpAddress::getLocalAddress();
    unsigned short copyPort = m_port;
    if (socket.receive(in, copyAddress, copyPort) == sf::Socket::Done) {
        std::cout << "From Server: " << std::endl;
    }
    return in;
}


/*!
 * Method to retrieve username of this UDPNetworkClient
 * @return std::string the username of this client
 */
std::string UDPNetworkClient::getUsername() {
    return username;
}

/*!
 * Method to set username of this UDPNetworkClient
 * @param new_name the new username
 * @return int representing success of operation (success = 0)
 */
int UDPNetworkClient::setUsername(std::string new_name) {
    username = new_name;
    return 0;
}

/*!
 * Method to handle client leaving / unbinding socket.
 * @return void
 */
void UDPNetworkClient::handleClientLeaving() {
    socket.unbind();
    exit(EXIT_SUCCESS);
}

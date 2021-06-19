/**
 *  @file   Packet.hpp
 *  @brief  Represents a packet of data to be sent between server and client.
 *  @author Mike and Team FunctionalPointers
 *  @date   2020-07-12
 ***********************************************/

#ifndef PAINTPACKET_HPP
#define PAINTPACKET_HPP

// Include our Third-Party SFML Header
#include <SFML/Network.hpp>

// Project header files
#include "Command.hpp"

// Other standard libraries
#include <string>
#include <vector>

// We can inherit from the SFML Packet and create our
// own custom packets that have our information in them.
// This gives us all of the functionality that we need
// when we want to send some custom information.
// 
// The pieces of information we want to send are the 
// client who last performed the action, and the
// command which they did. Then when the server receives
// these commands, it can store them, and broadcast this
// information to all of the other clients who have
// previously connected.
class myPacket : public sf::Packet {
public:
    /*!
     * The client for this packet.
     */
    std::string m_client;

    /*!
     * The command of this packet.
     */
    Command *m_command;

    /*!
     * Constuctor for Packet myPacket.
     */
    myPacket() {
        m_client = "whichever client I am";
        m_command = nullptr;
    }

    /*!
     * Destructor for Packet myPacket.
     */
    ~myPacket() {
        if (m_command != nullptr) {
            //delete m_command;
        }
    }
};

#endif

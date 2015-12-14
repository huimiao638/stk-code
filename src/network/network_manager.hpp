//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2013-2015 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/*! \file network_manager.hpp
 *  \brief Instantiates the generic functionnalities of a network manager.
 */

#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include "network/stk_peer.hpp"
#include "network/stk_host.hpp"

#include "network/protocol_manager.hpp"
#include "network/types.hpp"
#include "utils/singleton.hpp"
#include "utils/synchronised.hpp"

#include <vector>

class Event;
class GameSetup;

/** \class NetworkManager
 *  \brief Gives the general functions to use network communication.
 *  This class is in charge of storing the peers connected to this host.
 *  It also stores the host, and brings the functions to send messages to peers.
 *  It automatically dispatches the events or packets it receives. This class
 *  also stores the public address when known and the player login.
 *  Here are defined some functions that will be specifically implemented by
 *  the ServerNetworkManager and the ClientNetworkManager.
 */
class NetworkManager : public AbstractSingleton<NetworkManager>
{
protected:
             NetworkManager();
    virtual ~NetworkManager();

    /** Pointer to the one stk host instance, which is used to do all
     *  network communication. */
    STKHost* m_localhost;

    /** The list of peers connected to this instance. */
    std::vector<STKPeer*> m_peers;

private:
    GameSetup* m_game_setup;

    /** This computer's public IP address. With lock since it can
     *  be updated from a separate thread. */
    Synchronised<TransportAddress> m_public_address;

    PlayerLogin m_player_login;


    friend class AbstractSingleton<NetworkManager>;
public:
    virtual void run();
    virtual void reset();
    virtual void abort();
    virtual bool connect(const TransportAddress& peer);
    virtual void propagateEvent(Event* event);
    virtual void sendPacket(const NetworkString& data,
                            bool reliable = true) = 0;
    virtual void sendPacket(STKPeer* peer,
                            const NetworkString& data,
                            bool reliable = true);
    virtual void sendPacketExcept(STKPeer* peer,
                                  const NetworkString& data,
                                  bool reliable = true);

    // Game related functions
    virtual GameSetup* setupNewGame();
    virtual void disconnected();
    virtual bool isServer() = 0;

    // raw data management
    void setPublicAddress(const TransportAddress& addr);
    void removePeer(STKPeer* peer);

    // getters
    // ------------------------------------------------------------------------
    /** Returns if a peer from the specified IP:port address
     *  already exists. */
    virtual bool peerExists(const TransportAddress& peer)
    {
        return m_localhost->peerExists(peer);
    }   // peerExists
    // --------------------------------------------------------------------
    virtual bool isConnectedTo(const TransportAddress& peer)
    {
        return m_localhost->isConnectedTo(peer);
    }   // isConnectedTo

    // --------------------------------------------------------------------
    inline bool isClient() { return !isServer(); }
    // --------------------------------------------------------------------
    STKHost* getHost() { return m_localhost; }
    // --------------------------------------------------------------------
    std::vector<STKPeer*> getPeers() { return m_peers; }
    // --------------------------------------------------------------------
    unsigned int getPeerCount() { return (int)m_peers.size(); }
    // --------------------------------------------------------------------
    /** Returns the public IP address (thread safe). The network manager
     *  is a friend of TransportAddress and so has access to the copy
     *  constructor, which is otherwise declared private. */
    const TransportAddress getPublicAddress()
    {
        m_public_address.lock();
        TransportAddress a;
        a.copy(m_public_address.getData());
        m_public_address.unlock();
        return a;
    } // getPublicAddress

    // --------------------------------------------------------------------
    /** Returns the current game setup. */
    GameSetup* getGameSetup() { return m_game_setup; }

};   // class NetworkManager

#endif // NETWORKMANAGER_HPP

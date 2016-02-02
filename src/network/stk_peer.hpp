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

/*! \file stk_peer.hpp
 *  \brief Defines functions to easily manipulate 8-bit network destinated strings.
 */

#ifndef STK_PEER_HPP
#define STK_PEER_HPP

#include "utils/no_copy.hpp"
#include "utils/types.hpp"

#include <enet/enet.h>

class NetworkPlayerProfile;
class NetworkString;
class TransportAddress;

/*! \class STKPeer
 *  \brief Represents a peer.
 *  This class is used to interface the ENetPeer structure.
 */
class STKPeer : public NoCopy
{
protected:
    /** Pointer to the corresponding ENet peer data structure. */
    ENetPeer* m_enet_peer;

    NetworkPlayerProfile* m_player_profile;

    /** The token of this client. */
    uint32_t m_client_server_token;

    /** True if the token for this peer has been set. */
    bool m_token_set;

public:
             STKPeer(ENetPeer *enet_peer);
    virtual ~STKPeer();

    virtual void sendPacket(const NetworkString& data, bool reliable = true);
    void disconnect();
    bool isConnected() const;
    bool exists() const;
    uint32_t getAddress() const;
    uint16_t getPort() const;
    bool isSamePeer(const STKPeer* peer) const;
    bool isSamePeer(const ENetPeer* peer) const;

    // ------------------------------------------------------------------------
    /** Sets the token for this client. */
    void setClientServerToken(const uint32_t& token)
    {
        m_client_server_token = token; 
        m_token_set = true; 
    }   // setClientServerToken
    // ------------------------------------------------------------------------
    void unsetClientServerToken() { m_token_set = false; }
    // ------------------------------------------------------------------------
    void setPlayerProfile(NetworkPlayerProfile* profile)
    {
        m_player_profile = profile; 
    }   // setPlayerProfile
    // ------------------------------------------------------------------------
    /** Returns the player profile of this peer. */
    NetworkPlayerProfile* getPlayerProfile() 
    {
        return m_player_profile;
    }   // getPlayerProfile
    // ------------------------------------------------------------------------
    /** Returns the token of this client. */
    uint32_t getClientServerToken() const { return m_client_server_token; }
    // ------------------------------------------------------------------------
    /** Returns if the token for this client is known. */
    bool isClientServerTokenSet() const { return m_token_set; }

};   // STKPeer

#endif // STK_PEER_HPP

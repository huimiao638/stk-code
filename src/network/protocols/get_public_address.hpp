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

#ifndef GET_PUBLIC_ADDRESS_HPP
#define GET_PUBLIC_ADDRESS_HPP

#include "network/protocol.hpp"

#include <string>

class STKHost;

class GetPublicAddress : public Protocol
{
    public:
        GetPublicAddress();
        virtual ~GetPublicAddress() {}

        virtual bool notifyEvent(Event* event) { return true; }
        virtual bool notifyEventAsynchronous(Event* event) { return true; }
        virtual void setup() { m_state = NOTHING_DONE; }
        virtual void update() {}
        virtual void asynchronousUpdate();

    private:
        void createStunRequest();
        std::string parseStunResponse();

        // Constants
        static const uint32_t m_stun_magic_cookie;
        static const int m_stun_server_port = 3478;

        enum STATE
        {
            NOTHING_DONE,
            STUN_REQUEST_SENT,
            EXITING
        };
        STATE m_state;
        uint8_t m_stun_tansaction_id[12];
        uint32_t m_stun_server_ip;
        STKHost* m_transaction_host;
};

#endif // GET_PUBLIC_ADDRESS_HPP

//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2016 SuperTuxKart-Team
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

#ifndef HEADER_SOCCER_AI_HPP
#define HEADER_SOCCER_AI_HPP

#include "karts/controller/arena_ai.hpp"

class SoccerWorld;
class Vec3;
class Item;

/** The actual soccer AI.
 * \ingroup controller
 */
class SoccerAI : public ArenaAI
{
private:
    /** Keep a pointer to world. */
    SoccerWorld *m_world;

    SoccerTeam m_cur_team;
    bool m_saving_ball;

    Vec3 correctBallPosition(const Vec3&);

    virtual void findClosestKart(bool use_difficulty);
    virtual void findTarget();
    virtual int  getCurrentNode() const;
    virtual bool isWaiting() const;
    virtual bool canSkid(float steer_fraction) { return m_saving_ball; }
public:
                 SoccerAI(AbstractKart *kart,
                          StateManager::ActivePlayer *player = NULL);
                ~SoccerAI();
    virtual void update      (float delta);
    virtual void reset       ();
};

#endif

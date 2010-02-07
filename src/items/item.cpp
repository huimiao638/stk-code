//  $Id$
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2004 Steve Baker <sjbaker1@airmail.net>
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

#include "items/item.hpp"

#include "graphics/irr_driver.hpp"
#include "karts/kart.hpp"
#include "utils/constants.hpp"
#include "utils/coord.hpp"
#include "utils/vec3.hpp"

Item::Item(ItemType type, const Vec3& xyz, const Vec3& normal,
           scene::IMesh* mesh, unsigned int item_id)
{
    setType(type);
    m_event_handler    = NULL;
    m_deactive_time    = 0;
    m_normal           = normal;
    // Sets heading to 0, and sets pitch and roll depending on the normal. */
    Vec3  hpr          = Vec3(0, normal);
    m_coord            = Coord(xyz, hpr);
    m_item_id          = item_id;
    m_original_type    = ITEM_NONE;
    m_collected        = false;
    m_time_till_return = 0.0f;  // not strictly necessary, see isCollected()
    m_original_mesh    = mesh;
    m_node             = irr_driver->addMesh(mesh);
    m_node->setPosition(xyz.toIrrVector());
    m_node->grab();
}   // Item

//-----------------------------------------------------------------------------
/** Sets the type of this item, but also derived values, e.g. m_rotate.
 *  (bubblegums do not return).
 *  \param type New type of the item.
 */
void Item::setType(ItemType type)
{
    m_type   = type;
    m_rotate = type!=ITEM_BUBBLEGUM;
}   // setType

//-----------------------------------------------------------------------------
/** Changes this item to be a new type for a certain amount of time.
 *  \param type New type of this item.
 *  \param mesh Mesh to use to display this item.
 */
void Item::switchTo(ItemType type, scene::IMesh *mesh)
{
    m_original_type    = m_type;
    setType(type);
    m_node->setMesh(mesh);
}   // switchTo

//-----------------------------------------------------------------------------
/** Switch  backs to the original item.
 */
void Item::switchBack()
{
    assert(m_original_type!=ITEM_NONE);
    setType(m_original_type);
    m_original_type = ITEM_NONE;
    m_node->setMesh(m_original_mesh);
}   // switchBack

//-----------------------------------------------------------------------------
/** Removes an item.
 */
Item::~Item()
{
    irr_driver->removeNode(m_node);
    m_node->drop();
}   // ~Item

//-----------------------------------------------------------------------------
/** Resets before a race (esp. if a race is restarted).
 */
void Item::reset()
{
    m_collected        = false;
    m_time_till_return = 0.0f;
    m_deactive_time    = 0.0f;
    if(m_original_type!=ITEM_NONE)
    {
        setType(m_original_type);
        m_original_type = ITEM_NONE;
    }
}   // reset

//-----------------------------------------------------------------------------
/** Sets which karts dropped an item. This is used to avoid that a kart is
 *  affected by its own items.
 *  \param parent Kart that dropped the item.
 */
void Item::setParent(Kart* parent)
{
    m_event_handler = parent;
    m_deactive_time = 1.5f;
}   // setParent

//-----------------------------------------------------------------------------
/** Updated the item - rotates it, takes care of items coming back into
 *  the game after it has been collected.
 *  \param dt Time step size.
 */
void Item::update(float dt)
{
    if(m_deactive_time > 0) m_deactive_time -= dt;
    
    if(m_collected)
    {
        m_time_till_return -= dt;
        if ( m_time_till_return > 0 )
        {
            Vec3 hell(m_coord.getXYZ());

            hell.setZ( (m_time_till_return>1.0f) ? -1000000.0f 
               : m_coord.getXYZ().getZ() - m_time_till_return / 2.0f);
            m_node->setPosition(hell.toIrrVector());
        }
        else
        {
            m_collected    = false;
        }   // T>0

    }
    else
    {   // not m_collected
        
        if(!m_rotate) return;
        // have it rotate
        Vec3 rotation(dt*M_PI, 0, 0);
        m_coord.setHPR(m_coord.getHPR()+rotation);
        m_node->setRotation(m_coord.getHPR().toIrrHPR());
        m_node->setPosition(m_coord.getXYZ().toIrrVector());
        return;

        static float t=0;
        t += dt;

        btQuaternion q(Vec3(0,0,1), t*0.1f);
        btQuaternion q_orig(m_normal, 0);
        btQuaternion result=q+q_orig;
        btMatrix3x3 m(result);
        float y, p, r;
        m.getEuler(y, p, r);
        m_node->setRotation(Vec3(y, p, r).toIrrHPR());
    }
}   // update

//-----------------------------------------------------------------------------
/** Is called when the item is hit by a kart.  It sets the flag that the item
 *  has been collected, and the time to return to the parameter. 
 *  \param t Time till the object reappears (defaults to 2 seconds).
 */
void Item::collected(float t)
{
    m_collected  = true;
    if(m_type==ITEM_BUBBLEGUM)
    {
        deactivate(0.5);
        // Set the time till reappear to -1 seconds --> the item will 
        // reappear immediately.
        m_time_till_return = -1;

    }
    else
    {
        // Note if the time is negative, in update the m_collected flag will
        // be automatically set to false again.
        m_time_till_return = t;
    }
}   // isCollected


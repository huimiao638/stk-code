//  $Id: Options.cxx,v 1.6 2004/08/20 19:29:13 jamesgregory Exp $
//
//  TuxKart - a fun racing game with go-kart
//  Copyright (C) 2004 Steve Baker <sjbaker1@airmail.net>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
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

#include "Options.h"
#include "tuxkart.h"
#include "WidgetSet.h"

Options::Options()
{
	menu_id = widgetSet -> varray(0);
	widgetSet -> start(menu_id, "Controls",  GUI_SML, MENU_CONTROLS, 0);
	widgetSet -> state(menu_id, "Display",  GUI_SML, 0, 0);
	widgetSet -> state(menu_id, "Sound",  GUI_SML, 0, 0);
	widgetSet -> space(menu_id);
	widgetSet -> space(menu_id);
	
	widgetSet -> layout(menu_id, 0, -1);
}

Options::~Options()
{
	widgetSet -> delete_widget(menu_id) ;
}
	
void Options::update(float dt)
{
	
	widgetSet -> timer(menu_id, dt) ;
	widgetSet -> paint(menu_id) ;
}

void Options::select()
{
	switch ( widgetSet -> token (widgetSet -> click()) )
	{
	case MENU_CONTROLS: guiStack.push_back(GUIS_CONFIGCONTROLS); break;
	default: break;
	}
}

void Options::keybd(const SDL_keysym& key)
{
	switch ( key.sym )
	{
	case SDLK_LEFT:    
	case SDLK_RIGHT:    
	case SDLK_UP:    
	case SDLK_DOWN:
		widgetSet -> pulse(widgetSet -> cursor(menu_id, key.sym), 1.2f);
		break;
		
	case SDLK_RETURN: select(); break;
	
	case SDLK_ESCAPE:
		guiStack.pop_back();
		
	default: break;
	}
}

void Options::point(int x, int y)
{
	widgetSet -> pulse(widgetSet -> point(menu_id, x, y), 1.2f);
}

void Options::stick(int x, int y)
{
	widgetSet -> pulse(widgetSet -> stick(menu_id, x, y), 1.2f);
}


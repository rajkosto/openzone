/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/ui/Keyboard.cc
 */

#include "stable.hh"

#include "client/ui/Keyboard.hh"

namespace oz
{
namespace client
{
namespace ui
{

Keyboard keyboard;

void Keyboard::init()
{
  aSet<char>( keys, 0, SDLK_LAST);
  aSet<char>( oldKeys, 0, SDLK_LAST );
  currKeys = reinterpret_cast<char*>( SDL_GetKeyState( null ) );
}

void Keyboard::prepare()
{
  aCopy( oldKeys, keys, SDLK_LAST );
  aCopy( keys, currKeys, SDLK_LAST );
}

}
}
}
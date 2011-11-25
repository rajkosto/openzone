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
 * @file client/Network.hpp
 */

#pragma once

#include "client/common.hpp"

#ifdef OZ_NETWORKING
# include <SDL_net.h>
#endif

namespace oz
{
namespace client
{

class Network
{
  private:

#ifdef OZ_NETWORKING
    TCPsocket socket;
#endif

  public:

    bool connect();
    void disconnect();

    void update();

};

extern Network network;

}
}

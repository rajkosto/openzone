/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file client/Client.hh
 *
 * Game initialisation and main loop.
 */

#pragma once

#include <client/Stage.hh>

#include <SDL.h>

namespace oz
{
namespace client
{

class Client
{
  private:

    static const int INIT_PHYSFS     = 0x00000001;
    static const int INIT_SDL        = 0x00000002;
    static const int INIT_SDL_TTF    = 0x00000004;
    static const int INIT_CONFIG     = 0x00000010;
    static const int INIT_WINDOW     = 0x00000020;
    static const int INIT_INPUT      = 0x00000040;
    static const int INIT_NETWORK    = 0x00000080;
    static const int INIT_LINGUA     = 0x00000100;
    static const int INIT_LIBRARY    = 0x00000200;
    static const int INIT_CONTEXT    = 0x00001000;
    static const int INIT_RENDER     = 0x00002000;
    static const int INIT_AUDIO      = 0x00004000;
    static const int INIT_STAGE_INIT = 0x00010000;
    static const int INIT_STAGE_LOAD = 0x00020000;
    static const int INIT_MAIN_LOOP  = 0x00040000;

    Stage* stage;
    int    initFlags;

    float  benchmarkTime;
    bool   isBenchmark;

    void printUsage( const char* invocationName );

  public:

    int init( int argc, char** argv );
    void shutdown();

    int main();

};

extern Client client;

}
}

/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file build/Context.hpp
 */

#pragma once

#include "build/common.hpp"

namespace oz
{
namespace build
{

class Context
{
  public:

    static const int DEFAULT_MAG_FILTER;
    static const int DEFAULT_MIN_FILTER;

  private:

    // texture reading buffer
    static const int BUFFER_SIZE          = 256 * 1024;
    // default audio format
    static const int DEFAULT_AUDIO_FREQ   = 44100;
    static const int DEFAULT_AUDIO_FORMAT = AUDIO_S16LSB;

  public:

    static bool useS3TC;

  private:

    static uint buildTexture( const void* data, int width, int height, uint format,
                              bool wrap, int magFilter, int minFilter );

  public:

    static uint createTexture( const void* data, int width, int height, uint format,
                               bool wrap = true, int magFilter = DEFAULT_MAG_FILTER,
                               int minFilter = DEFAULT_MIN_FILTER );

    static uint loadRawTexture( const char* path, bool wrap = true,
                                int magFilter = DEFAULT_MAG_FILTER,
                                int minFilter = DEFAULT_MIN_FILTER );

    static void writeTexture( uint id, OutputStream* stream );

};

}
}
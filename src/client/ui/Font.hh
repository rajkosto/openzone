/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file client/ui/Font.hh
 */

#pragma once

#include <client/common.hh>

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

namespace oz
{
namespace client
{
namespace ui
{

class Font
{
  public:

    enum Type
    {
      MONO,
      SANS,
      SMALL,
      LARGE,
      TITLE,
      MAX
    };

  private:

    Buffer    buffer;
    TTF_Font* handle;

  public:

    int height;

  public:

    int sizeOf( const char* s ) const;
    void upload( const char* s, int* width, int* height ) const;

    void init( const char* name, int height );
    void destroy();

};

}
}
}

/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file client/common.hh
 */

#pragma once

#include <matrix/Liber.hh>
#include <matrix/Collider.hh>
#include <nirvana/common.hh>
#include <ozEngine/ozEngine.hh>

#include <client/config.hh>

// SDL macro named `main` causes havoc in several places in OpenZone engine.
#define SDL_MAIN_HANDLED

namespace oz
{
namespace client
{

struct TexCoord
{
  static const TexCoord ZERO;

  float u = 0.0f;
  float v = 0.0f;

  OZ_ALWAYS_INLINE
  TexCoord() = default;

  OZ_ALWAYS_INLINE
  explicit TexCoord(float u_, float v_) :
    u(u_), v(v_)
  {}

  OZ_ALWAYS_INLINE
  bool operator == (const TexCoord& tc) const
  {
    return u == tc.u && v == tc.v;
  }

  OZ_ALWAYS_INLINE
  bool operator != (const TexCoord& tc) const
  {
    return !operator == (tc);
  }

  OZ_ALWAYS_INLINE
  operator const float* () const
  {
    return &u;
  }

  OZ_ALWAYS_INLINE
  operator float* ()
  {
    return &u;
  }

  OZ_ALWAYS_INLINE
  const float& operator [] (int i) const
  {
    hard_assert(0 <= i && i < 2);

    return (&u)[i];
  }

  OZ_ALWAYS_INLINE
  float& operator [] (int i)
  {
    hard_assert(0 <= i && i < 2);

    return (&u)[i];
  }
};

extern const Json::Format CONFIG_FORMAT;

extern Collider collider;
extern Json     config;
extern File     configDir;

}
}

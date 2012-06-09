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
 * @file build/Mesh.hh
 */

#pragma once

#include "client/Mesh.hh"

#include "build/common.hh"

namespace oz
{
namespace build
{

struct Vertex
{
  Point    pos;
  TexCoord texCoord;
  Vec3     normal;
  Vec3     tangent;
  Vec3     binormal;
  int      bones[2];
  float    blend;

  bool operator == ( const Vertex& v ) const;

  void write( BufferStream* ostream ) const;
};

class MeshData
{
  friend class Compiler;

  public:

    struct Part
    {
      int    component;
      uint   mode;

      int    material;
      String texture;

      int    nIndices;
      int    firstIndex;
    };

    Vector<Part>   parts;

    DArray<ushort> indices;
    DArray<Vertex> vertices;

    void write( BufferStream* os, bool embedTextures = true ) const;

};

}
}

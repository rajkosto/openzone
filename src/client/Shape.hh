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
 * @file client/Shape.hh
 */

#pragma once

#include "matrix/Frag.hh"

#include "client/Mesh.hh"

namespace oz
{
namespace client
{

class Shape
{
  private:

    struct ShapeVertex
    {
      float pos[3];
      float texCoord[2];
      float normal[3];
    };

    static const ShapeVertex VERTICES[];
    static const ushort INDICES[];

    uint vao;
    uint vbo;
    uint ibo;

  public:

    Shape();

    void bindVertexArray() const;

    static void fill( float x, float y, float width, float height );
    static void fill( int x, int y, int width, int height );
    static void fillInv( float x, float y, float width, float height );
    static void fillInv( int x, int y, int width, int height );
    static void rect( float x, float y, float width, float height );
    static void rect( int x, int y, int width, int height );
    static void tag( float minX, float minY, float maxX, float maxY );

    static void quad( float dimX, float dimY );
    static void box( const AABB& bb );
    static void wireBox( const AABB& bb );

    void load();
    void unload();

};

extern Shape shape;

}
}

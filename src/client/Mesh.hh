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
 * @file client/Mesh.hh
 */

#pragma once

#include "client/common.hh"

namespace oz
{
namespace client
{

struct Vertex
{
  float pos[3];
  float texCoord[2];
  float normal[3];
#ifdef OZ_BUMPMAP
  float tangent[3];
  float binormal[3];
  float detailCoord[2];
#endif

  void read( InputStream* istream );

  static void setFormat();
};

struct Texture
{
  uint albedo;
  uint masks;
  uint normals;

  void free();
};

class Mesh
{
  public:

    static const int COMPONENT_MASK  = 0x00ff; ///< Mask to get number of mesh components.
    static const int EMBEDED_TEX_BIT = 0x0100; ///< Textures are embedded in SMM file.
    static const int SOLID_BIT       = 0x0200; ///< Mesh component is not transparent.
    static const int ALPHA_BIT       = 0x0400; ///< Mesh component is transparent.

    static const int ALBEDO_BIT      = 0x1000; ///< Texture has albedo (base per-pixel colours).
    static const int MASKS_BIT       = 0x2000; ///< Texture has specular and emission masks.
    static const int NORMALS_BIT     = 0x4000; ///< Texture has normal map.

  private:

    struct Part
    {
      int     flags;
      uint    mode;
      Texture texture;

      int     nIndices;
      int     firstIndex;
    };

    static const Mesh* lastMesh;

    uint         vao;
    uint         vbo;
    uint         ibo;

    int          flags;
    int          nParts;
    Part*        parts;
    DArray<int>  texIds;

  public:

    static void reset();

    Mesh();
    ~Mesh();

    void load( InputStream* istream, uint usage, const char* path );
    void unload();

    void upload( const Vertex* vertices, int nVertices, uint usage ) const;

    void bind() const;
    void drawComponent( int id, int mask ) const;
    void draw( int mask ) const;

};

}
}

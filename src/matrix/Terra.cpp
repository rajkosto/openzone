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
 * @file matrix/Terra.cpp
 */

#include "stable.hpp"

#include "matrix/Terra.hpp"

#include "matrix/Library.hpp"

namespace oz
{
namespace matrix
{

const float Terra::Quad::SIZE     = float( SIZEI );
const float Terra::Quad::INV_SIZE = 1.0f / float( SIZEI );
const float Terra::Quad::DIM      = SIZE / 2.0f;

const float Terra::DIM            = Terra::Quad::DIM * QUADS;

Terra::Terra() : id( -1 )
{}

void Terra::load( int id_ )
{
  id = id_;

  const String& name = library.terras[id].name;
  const String& path = library.terras[id].path;

  log.print( "Loading terrain '%s' ...", name.cstr() );

  File file( path );
  if( !file.map() ) {
    log.printEnd( " Cannot read file" );
    throw Exception( "Failed to load terrain" );
  }

  InputStream is = file.inputStream();

  int max = is.readInt();
  if( max != VERTS ) {
    log.printEnd( " Invalid dimension %d, should be %d", max, VERTS );
    throw Exception( "Failed to load terrain" );
  }

  for( int x = 0; x < VERTS; ++x ) {
    for( int y = 0; y < VERTS; ++y ) {
      quads[x][y].vertex       = is.readPoint3();
      quads[x][y].triNormal[0] = is.readVec3();
      quads[x][y].triNormal[1] = is.readVec3();
    }
  }

  file.unmap();

  log.printEnd( " OK" );
}

void Terra::init()
{
  for( int x = 0; x < VERTS; ++x ) {
    for( int y = 0; y < VERTS; ++y ) {
      quads[x][y].vertex.x = float( x * Quad::SIZEI ) - DIM;
      quads[x][y].vertex.y = float( y * Quad::SIZEI ) - DIM;
      quads[x][y].vertex.z = 0.0f;
      quads[x][y].triNormal[0] = Vec3::ZERO;
      quads[x][y].triNormal[1] = Vec3::ZERO;
    }
  }
}

void Terra::read( InputStream* istream )
{
  String name = istream->readString();
  int id = library.terraIndex( name );

  load( id );
}

void Terra::write( BufferStream* ostream ) const
{
  ostream->writeString( library.terras[id].name );
}

}
}

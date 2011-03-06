/*
 *  Mesh.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Compiler.hpp"

#ifdef OZ_BUILD_TOOLS

#include "client/Colours.hpp"
#include "client/Context.hpp"

#include <GL/gl.h>

namespace oz
{
namespace client
{

  Compiler compiler;

  bool Compiler::Part::operator == ( const Part& part ) const
  {
    return mode == part.mode &&
        diffuse == part.diffuse &&
        specular == part.specular &&
        texture[0].equals( part.texture[0] ) &&
        texture[1].equals( part.texture[1] ) &&
        texture[2].equals( part.texture[2] );
  }

  void Compiler::enable( int cap )
  {
    caps |= cap;
  }

  void Compiler::disable( int cap )
  {
    caps &= ~cap;
  }

  void Compiler::beginMesh()
  {
    hard_assert( ~flags & MESH_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    flags |= MESH_BIT;
    caps = 0;

    vertices.clear();
    solidParts.clear();
    alphaParts.clear();

    part.diffuse    = Colours::WHITE;
    part.specular   = Colours::BLACK;
    part.texture[0] = "";
    part.texture[1] = "";
    part.texture[2] = "";

    vert.pos[0] = 0.0f;
    vert.pos[1] = 0.0f;
    vert.pos[2] = 0.0f;

    vert.texCoord[0] = 0.0f;
    vert.texCoord[1] = 0.0f;

    vert.normal[0] = 0.0f;
    vert.normal[1] = 0.0f;
    vert.normal[2] = 0.0f;
  }

  void Compiler::endMesh()
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    flags &= ~MESH_BIT;
  }

  void Compiler::material( int, int target, const float* params )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    switch( target ) {
      case GL_DIFFUSE: {
        part.diffuse.x = params[0];
        part.diffuse.y = params[1];
        part.diffuse.z = params[2];
        part.diffuse.w = params[3];
        break;
      }
      case GL_SPECULAR: {
        part.specular.x = params[0];
        part.specular.y = params[1];
        part.specular.z = params[2];
        part.specular.w = params[3];
        break;
      }
      default: {
        hard_assert( false );
        break;
      }
    }
  }

  void Compiler::texture( int unit, const char* texture )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    hard_assert( 0 <= unit && unit <= 2 );

    part.texture[unit] = texture;
  }

  void Compiler::begin( int mode_ )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    flags |= SURFACE_BIT;

    mode = mode_;
    vertNum = 0;

    switch( mode ) {
      case GL_QUADS:
      case GL_POLYGON: {
        part.mode = GL_TRIANGLE_STRIP;
        break;
      }
      default: {
        part.mode = mode;
        break;
      }
    }
  }

  void Compiler::end()
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & SURFACE_BIT );

    flags &= ~SURFACE_BIT;

    if( caps & CAP_CW ) {
      aReverse<int>( part.indices, part.indices.length() );
    }

    switch( mode ) {
      case GL_POINTS: {
        hard_assert( vertNum >= 1 );
        break;
      }
      case GL_TRIANGLE_STRIP: {
        hard_assert( vertNum >= 3 );
        break;
      }
      case GL_TRIANGLE_FAN: {
        hard_assert( vertNum >= 3 );
        break;
      }
      case GL_TRIANGLES: {
        hard_assert( vertNum >= 3 && vertNum % 3 == 0 );
        break;
      }
      case GL_QUADS: {
        hard_assert( vertNum >= 4 && vertNum % 4 == 0 );

        for( int i = 0; i < vertNum; i += 4 ) {
          swap( part.indices[i + 2], part.indices[i + 3] );
        }
        break;
      }
      case GL_POLYGON: {
        hard_assert( vertNum >= 3 );

        int n_2 = ( vertNum - 2 ) / 2;
        for( int i = 1; i <= n_2; ++i ) {
          int index = part.indices.last();
          aInsert<int>( part.indices, index, 2 * i, part.indices.length() );
        }
        break;
      }
    }

    Vector<Part>* parts = part.diffuse.w != 1.0f ? &alphaParts : &solidParts;

    switch( mode ) {
      default: {
        int partIndex = parts->index( part );

        if( partIndex == -1 ) {
          parts->add( part );
        }
        else {
          Part* destPart = &( *parts )[partIndex];

          switch( part.mode ) {
            case GL_TRIANGLE_STRIP: {
              // reset triangle strip
              destPart->indices.add( destPart->indices.last() );
              destPart->indices.add( part.indices.first() );
              break;
            }
          }
          destPart->indices.addAll( part.indices, part.indices.length() );
        }
        break;
      }
    }

    part.indices.clear();
  }

  void Compiler::texCoord( float u, float v )
  {
    hard_assert( flags & MESH_BIT );

    vert.texCoord[0] = u;
    vert.texCoord[1] = v;
  }

  void Compiler::texCoord( const float* v )
  {
    texCoord( v[0], v[1] );
  }

  void Compiler::normal( float nx, float ny, float nz )
  {
    hard_assert( flags & MESH_BIT );

    vert.normal[0] = nx;
    vert.normal[1] = ny;
    vert.normal[2] = nz;
  }

  void Compiler::normal( const float* v )
  {
    normal( v[0], v[1], v[2] );
  }

  void Compiler::vertex( float x, float y, float z )
  {
    hard_assert( flags & MESH_BIT );

    vert.pos[0] = x;
    vert.pos[1] = y;
    vert.pos[2] = z;

    if( ~flags & SURFACE_BIT ) {
      vertices.add( vert );
    }
    else {
      bool doRestart = false;

      if( mode == GL_QUADS && vertNum != 0 && vertNum % 4 == 0 ) {
        doRestart = true;
      }

      if( doRestart ) {
        part.indices.add( part.indices.last() );
      }

      int index;

      if( caps & CAP_UNIQUE ) {
        index = vertices.include( vert );
        part.indices.add( index );
      }
      else {
        index = vertices.length();

        vertices.add( vert );
        part.indices.add( index );
      }

      if( doRestart ) {
        part.indices.add( index );
      }

      ++vertNum;
    }
  }

  void Compiler::vertex( const float* v )
  {
    vertex( v[0], v[1], v[2] );
  }

  void Compiler::index( int i )
  {
    hard_assert( flags & MESH_BIT );
    hard_assert( flags & SURFACE_BIT );

    bool doRestart = false;

    if( mode == GL_QUADS && vertNum != 0 && vertNum % 4 == 0 ) {
      doRestart = true;
    }

    if( doRestart ) {
      part.indices.add( part.indices.last() );
    }

    part.indices.add( i );

    if( doRestart ) {
      part.indices.add( i );
    }

    ++vertNum;
  }

  void Compiler::getMeshData( MeshData* mesh ) const
  {
    hard_assert( ~flags & MESH_BIT );
    hard_assert( ~flags & SURFACE_BIT );

    int nIndices = 0;

    for( int i = 0; i < solidParts.length(); ++i ) {
      mesh->solidParts.add();

      mesh->solidParts[i].diffuse    = solidParts[i].diffuse;
      mesh->solidParts[i].specular   = solidParts[i].specular;
      mesh->solidParts[i].texture[0] = solidParts[i].texture[0];
      mesh->solidParts[i].texture[1] = solidParts[i].texture[1];
      mesh->solidParts[i].texture[2] = solidParts[i].texture[2];

      mesh->solidParts[i].mode       = solidParts[i].mode;

      mesh->solidParts[i].nIndices   = solidParts[i].indices.length();
      mesh->solidParts[i].firstIndex = nIndices;

      nIndices += solidParts[i].indices.length();
    }

    for( int i = 0; i < alphaParts.length(); ++i ) {
      mesh->alphaParts.add();

      mesh->alphaParts[i].diffuse    = alphaParts[i].diffuse;
      mesh->alphaParts[i].specular   = alphaParts[i].specular;
      mesh->alphaParts[i].texture[0] = alphaParts[i].texture[0];
      mesh->alphaParts[i].texture[1] = alphaParts[i].texture[1];
      mesh->alphaParts[i].texture[2] = alphaParts[i].texture[2];

      mesh->alphaParts[i].mode       = alphaParts[i].mode;

      mesh->alphaParts[i].nIndices   = alphaParts[i].indices.length();
      mesh->alphaParts[i].firstIndex = nIndices;

      nIndices += alphaParts[i].indices.length();
    }

    mesh->indices.alloc( nIndices );
    ushort* currIndex = mesh->indices;

    foreach( part, solidParts.citer() ) {
      foreach( i, part->indices.citer() ) {
        *currIndex = ushort( *i );
        ++currIndex;
      }
    }
    foreach( part, alphaParts.citer() ) {
      foreach( i, part->indices.citer() ) {
        *currIndex = ushort( *i );
        ++currIndex;
      }
    }

    mesh->vertices.alloc( vertices.length() );
    aCopy<Vertex>( mesh->vertices, vertices, vertices.length() );
  }

  void Compiler::free()
  {
    vertices.clear();
    vertices.dealloc();

    solidParts.clear();
    solidParts.dealloc();

    alphaParts.clear();
    alphaParts.dealloc();

    part.indices.clear();
    part.indices.dealloc();
  }

}
}

#endif

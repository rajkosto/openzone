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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/FragPool.cpp
 */

#include "stable.hpp"

#include "client/FragPool.hpp"

#include "client/Shader.hpp"
#include "client/Mesh.hpp"
#include "client/Context.hpp"
#include "client/SMM.hpp"
#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

const float FragPool::FRAG_RADIUS = 1.0f;

FragPool::FragPool( const matrix::FragPool* pool_ ) : pool( pool_ ), flags( 0 )
{
  log.print( "Loading FragPool '%s' ...", pool->name.cstr() );

  models.alloc( pool->models.length() );

  for( int i = 0; i < pool->models.length(); ++i ) {
    models.add( context.requestSMM( pool->models[i] ) );
  }

  log.printEnd( " OK" );
}

FragPool::~FragPool()
{
  log.print( "Unloading FragPool '%s' ...", pool->name.cstr() );

  for( int i = 0; i < pool->models.length(); ++i ) {
    context.releaseSMM( pool->models[i] );
  }

  log.printEnd( " OK" );
}

void FragPool::draw( const Frag* frag )
{
  flags |= UPDATED_BIT;

  int  index = frag->index % models.length();
  SMM* model = models[index];

  if( model->isLoaded ) {
    tf.model = Mat44::translation( frag->p - Point3::ORIGIN );
    tf.model.rotateX( frag->p.x );
    tf.model.rotateY( frag->p.y );
    tf.model.rotateZ( frag->p.z );

    shader.colour = Vec4( 1.0f, 1.0f, 1.0f, clamp( frag->life, 0.0f, 1.0f ) );

    model->draw( Mesh::SOLID_BIT | Mesh::ALPHA_BIT );
  }
}

}
}

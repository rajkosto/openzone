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
 * @file client/MD2Imago.cc
 */

#include <stable.hh>
#include <client/MD2Imago.hh>

#include <client/Context.hh>
#include <client/Camera.hh>

namespace oz
{
namespace client
{

const float MD2Imago::TURN_SMOOTHING_COEF = 0.25f;

Pool<MD2Imago, 256> MD2Imago::pool;

MD2Imago::~MD2Imago()
{
  context.releaseMD2( clazz->imagoModel );
}

Imago* MD2Imago::create( const Object* obj )
{
  hard_assert( obj->flags & Object::BOT_BIT );

  const Bot* bot   = static_cast<const Bot*>( obj );
  MD2Imago*  imago = new MD2Imago( obj );

  imago->flags = Imago::MD2MODEL_BIT;
  imago->md2   = context.requestMD2( obj->clazz->imagoModel );
  imago->h     = bot->h;

  return imago;
}

void MD2Imago::draw( const Imago* parent )
{
  if( !md2->isLoaded ) {
    return;
  }

  const Bot*      bot   = static_cast<const Bot*>( obj );
  const BotClass* clazz = static_cast<const BotClass*>( bot->clazz );

  anim.advance();

  if( bot->state & Bot::DEAD_BIT ) {
    if( shader.mode == Shader::SCENE && parent == nullptr ) {
      Vec3 t = Vec3( obj->p.x, obj->p.y, obj->p.z + clazz->dim.z - clazz->corpseDim.z );

      tf.model = Mat44::translation( t );
      tf.model.rotateZ( h );

      tf.colour.w.w = min( bot->life * 8.0f / clazz->life, 1.0f );
    }

    md2->scheduleAnim( &anim );

    // FIXME Enable when no buggy models are used (no mismatched death animation for weapons).
//     if( parent == nullptr && bot->weapon >= 0 && orbis.objects[bot->weapon] != nullptr ) {
//       context.drawImago( orbis.objects[bot->weapon], this, Mesh::SOLID_BIT );
//     }
  }
  else if( bot->index == camera.bot && !camera.isExternal ) {
    h = bot->h;

    if( parent == nullptr && bot->weapon >= 0 && orbis.objects[bot->weapon] != nullptr ) {
      tf.model = Mat44::translation( obj->p - Point::ORIGIN );
      tf.model.rotateZ( bot->h );

      tf.model.translate( Vec3( 0.0f, 0.0f, +bot->camZ ) );
      tf.model.rotateX( bot->v - Math::TAU / 4.0f );
      tf.model.translate( Vec3( 0.0f, 0.0f, -bot->camZ ) );

      glDepthFunc( GL_ALWAYS );

      context.drawImago( orbis.objects[bot->weapon], this );

      glDepthFunc( GL_LEQUAL );
    }
  }
  else {
    if( shader.mode == Shader::SCENE && parent == nullptr ) {
      h = angleWrap( h + TURN_SMOOTHING_COEF * angleDiff( bot->h, h ) );

      tf.model = Mat44::translation( obj->p - Point::ORIGIN );
      tf.model.rotateZ( h );

      if( bot->state & Bot::CROUCHING_BIT ) {
        tf.model.translate( Vec3( 0.0f, 0.0f, clazz->dim.z - clazz->crouchDim.z ) );
      }
    }

    md2->scheduleAnim( &anim );

    if( parent == nullptr && bot->weapon >= 0 && orbis.objects[bot->weapon] != nullptr ) {
      context.drawImago( orbis.objects[bot->weapon], this );
    }
  }

  tf.colour.w.w = 1.0f;
}

}
}

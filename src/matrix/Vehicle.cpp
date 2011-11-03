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
 * @file matrix/Vehicle.cpp
 */

#include "stable.hpp"

#include "matrix/Vehicle.hpp"

#include "matrix/Collider.hpp"
#include "matrix/Physics.hpp"
#include "matrix/VehicleClass.hpp"
#include "matrix/Bot.hpp"
#include "matrix/Lua.hpp"

namespace oz
{
namespace matrix
{

const float Vehicle::AIR_FRICTION   = 0.02f;
const float Vehicle::EXIT_EPSILON   = 0.20f;
const float Vehicle::EXIT_MOMENTUM  = 1.00f;
const float Vehicle::EJECT_MOMENTUM = 15.0f;

Pool<Vehicle, 256> Vehicle::pool;

void ( Vehicle::* Vehicle::handlers[Vehicle::TYPE_MAX] )( const Mat44& rotMat ) = {
  &Vehicle::staticHandler,
  &Vehicle::wheeledHandler,
  &Vehicle::trackedHandler,
  &Vehicle::mechHandler,
  &Vehicle::hoverHandler,
  &Vehicle::airHandler
};

void Vehicle::staticHandler( const Mat44& )
{}

void Vehicle::wheeledHandler( const Mat44& )
{}

void Vehicle::trackedHandler( const Mat44& )
{}

void Vehicle::mechHandler( const Mat44& )
{}

void Vehicle::hoverHandler( const Mat44& )
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( h, &hvsc[0], &hvsc[1] );
  Math::sincos( v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

  // raycast for hover
  float ratio = clamp( p.z / ( dim.z + clazz->hoverHeight ), 0.0f, 1.0f );
  Vec3  floor = Vec3( 0.0f, 0.0f, 1.0f );

  collider.translate( p, Vec3( 0.0f, 0.0f, -dim.z - clazz->hoverHeight ) );

  if( collider.hit.ratio < ratio ) {
    ratio = collider.hit.ratio;
    floor = collider.hit.normal;
  }

  float ratio_1 = 1.0f - ratio;

  // controls
  Vec3 move = Vec3::ZERO;

  if( actions & Bot::ACTION_FORWARD ) {
    move.x -= hvsc[0];
    move.y += hvsc[1];
  }
  if( actions & Bot::ACTION_BACKWARD ) {
    move.x += hvsc[0];
    move.y -= hvsc[1];
  }
  if( actions & Bot::ACTION_RIGHT ) {
    move.x += hvsc[1];
    move.y += hvsc[0];
  }
  if( actions & Bot::ACTION_LEFT ) {
    move.x -= hvsc[1];
    move.y -= hvsc[0];
  }

  momentum += move * clazz->moveMomentum;
  momentum.x *= 1.0f - AIR_FRICTION;
  momentum.y *= 1.0f - AIR_FRICTION;

  // hover momentum
  if( ratio_1 != 0.0f ) {
    float groundMomentum = min( velocity * floor, 0.0f );
    float tickRatio = ratio_1*ratio_1 * Timer::TICK_TIME;

    momentum.z += clazz->hoverHeightStiffness * tickRatio;
    momentum.z -= groundMomentum * clazz->hoverMomentumStiffness * min( tickRatio / 4.0f, 1.0f );
  }
}

void Vehicle::airHandler( const Mat44& rotMat )
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  const Vec3& right = rotMat.x;
  const Vec3& at    = rotMat.y;
  const Vec3& up    = rotMat.z;

  // controls
  Vec3 move = Vec3::ZERO;

  if( actions & Bot::ACTION_FORWARD ) {
    move += at;
  }
  if( actions & Bot::ACTION_BACKWARD ) {
    move -= at;
  }
  if( actions & Bot::ACTION_RIGHT ) {
    move += right;
  }
  if( actions & Bot::ACTION_LEFT ) {
    move -= right;
  }
  if( actions & Bot::ACTION_VEH_UP ) {
    move += up;
  }
  if( actions & Bot::ACTION_VEH_DOWN ) {
    move -= up;
  }

  momentum += move * clazz->moveMomentum;
  momentum.z -= Physics::G_ACCEL * Timer::TICK_TIME;
  momentum *= 1.0f - AIR_FRICTION;
}

void Vehicle::onDestroy()
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  Mat44 rotMat = Mat44::rotation( rot );

  if( pilot != -1 ) {
    Bot* bot = static_cast<Bot*>( orbis.objects[pilot] );

    pilot = -1;

    if( bot != null ) {
      bot->p = p + rotMat * clazz->pilotPos;
      bot->p.z += dim.z + EXIT_EPSILON;

      // kill bot if eject path is blocked
      if( collider.overlaps( *bot, this ) ) {
        bot->kill();
        bot->exit();
      }
      else {
        float hsc[2];
        Math::sincos( h, &hsc[0], &hsc[1] );

        bot->momentum += EJECT_MOMENTUM * ~Vec3( hsc[0], -hsc[1], 0.10f );
        bot->exit();
      }
    }
  }
  Object::onDestroy();
}

void Vehicle::onUpdate()
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  // clean invalid pilot reference and throw him out if dead
  if( pilot != -1 ) {
    Bot* bot = static_cast<Bot*>( orbis.objects[pilot] );

    if( bot == null || bot->parent == -1 ) {
      pilot = -1;
    }
    else if( bot->flags & Bot::DEAD_BIT ) {
      pilot = -1;
      bot->exit();
    }
  }

  actions = 0;

  Bot* bot = null;

  if( pilot != -1 ) {
    bot = static_cast<Bot*>( orbis.objects[pilot] );

    h = bot->h;
    v = bot->v;
    rot = Quat::rotZYX( h, 0.0f, v - Math::TAU / 4.0f );
    actions = bot->actions;
    flags &= ~DISABLED_BIT;
  }

  Mat44 rotMat = Mat44::rotation( rot );

  if( pilot != -1 ) {
    ( this->*handlers[clazz->type] )( rotMat );
  }

  // move forwards (predicted movement) to prevent our bullets hitting us in the back when we are
  // moving very fast
  Point3 oldPos = p;
  p += momentum * Timer::TICK_TIME;

  if( clazz->nWeapons != 0 ) {
    if( actions & ~oldActions & Bot::ACTION_VEH_NEXT_WEAPON ) {
      weapon = ( weapon + 1 ) % clazz->nWeapons;
      addEvent( EVENT_NEXT_WEAPON, 1.0f );
    }
    if( actions & Bot::ACTION_ATTACK ) {
      if( shotTime[weapon] == 0.0f ) {
        shotTime[weapon] = clazz->shotInterval[weapon];

        if( nRounds[weapon] == 0 ) {
          addEvent( EVENT_SHOT0_EMPTY + weapon*2, 1.0f );
        }
        else {
          nRounds[weapon] = max( -1, nRounds[weapon] - 1 );

          addEvent( EVENT_SHOT0 + weapon*2, 1.0f );
          lua.objectCall( clazz->onShot[weapon], this, bot );
        }
      }
    }
  }

  p = oldPos;

  for( int i = 0; i < clazz->nWeapons; ++i ) {
    if( shotTime[i] > 0.0f ) {
      shotTime[i] = max( shotTime[i] - Timer::TICK_TIME, 0.0f );
    }
  }

  if( bot != null ) {
    bot->p = p + rotMat * clazz->pilotPos + momentum * Timer::TICK_TIME;
    bot->momentum = velocity;
    bot->velocity = velocity;

    if( bot->actions & Bot::ACTION_EXIT ) {
      float hsc[2];
      Math::sincos( h, &hsc[0], &hsc[1] );

      float  handle = !( dim + bot->dim ) + EXIT_EPSILON;
      Point3 exitPos = Point3( p.x - hsc[0] * handle, p.y + hsc[1] * handle, p.z + dim.z );

      if( !collider.overlaps( AABB( exitPos, bot->dim ) ) ) {
        pilot = -1;

        bot->p = exitPos;
        bot->exit();
      }
    }
    else if( bot->actions & Bot::ACTION_EJECT ) {
      bot->p = p + rotMat * clazz->pilotPos;

      Point3 ejectPos = Point3( p.x, p.y, p.z + dim.z + bot->dim.z + EXIT_EPSILON );

      pilot = -1;

      // kill bot if eject path is blocked
      if( collider.overlaps( AABB( ejectPos, bot->dim ) ) ) {
        bot->kill();
        bot->exit();
      }
      else {
        float hsc[2];
        Math::sincos( h, &hsc[0], &hsc[1] );

        bot->p = ejectPos;
        bot->momentum += EJECT_MOMENTUM * ~Vec3( hsc[0], -hsc[1], 0.10f );
        bot->exit();
      }
    }
  }

  oldActions = actions;
  oldState   = state;
}

bool Vehicle::onUse( Bot* user )
{
  if( pilot == -1 ) {
    pilot = user->index;

    user->h = h;
    user->v = v;
    user->enter( index );

    return true;
  }
  return false;
}

Vehicle::Vehicle() : actions( 0 ), oldActions( 0 ), weapon( 0 ), pilot( -1 )
{}

void Vehicle::service()
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  if( life != clazz->life ) {
    life = clazz->life;
  }

  for( int i = 0; i < clazz->nWeapons; ++i ) {
    if( nRounds[i] != clazz->nRounds[i] ) {
      nRounds[i] = clazz->nRounds[i];
    }
  }
}

void Vehicle::readFull( InputStream* istream )
{
  Dynamic::readFull( istream );

  h          = istream->readFloat();
  v          = istream->readFloat();
  rot        = istream->readQuat();
  state      = istream->readInt();
  oldState   = istream->readInt();
  actions    = istream->readInt();
  oldActions = istream->readInt();

  weapon     = istream->readInt();

  for( int i = 0; i < WEAPONS_MAX; ++i ) {
    nRounds[i]  = istream->readInt();
    shotTime[i] = istream->readFloat();
  }

  pilot = istream->readInt();
}

void Vehicle::writeFull( BufferStream* ostream ) const
{
  Dynamic::writeFull( ostream );

  ostream->writeFloat( h );
  ostream->writeFloat( v );
  ostream->writeQuat( rot );
  ostream->writeInt( state );
  ostream->writeInt( oldState );
  ostream->writeInt( actions );
  ostream->writeInt( oldActions );

  ostream->writeInt( weapon );

  for( int i = 0; i < WEAPONS_MAX; ++i ) {
    ostream->writeInt( nRounds[i] );
    ostream->writeFloat( shotTime[i] );
  }

  ostream->writeInt( pilot );
}

void Vehicle::readUpdate( InputStream* istream )
{
  Dynamic::readUpdate( istream );

  rot    = istream->readQuat();
  state  = istream->readInt();

  weapon = istream->readInt();

  for( int i = 0; i < WEAPONS_MAX; ++i ) {
    nRounds[i] = istream->readInt();
  }

  pilot = istream->readInt();
}

void Vehicle::writeUpdate( BufferStream* ostream ) const
{
  Dynamic::writeUpdate( ostream );

  ostream->writeQuat( rot );
  ostream->writeInt( state );

  ostream->writeInt( weapon );

  for( int i = 0; i < WEAPONS_MAX; ++i ) {
    ostream->writeInt( nRounds[i] );
  }

  ostream->writeInt( pilot );
}

}
}

/*
 *  Object.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Object.h"

#include "ObjectClass.h"
#include "MatrixLua.h"
#include "Synapse.h"

namespace oz
{

  Object::~Object()
  {
    assert( dim.x <= AABB::REAL_MAX_DIM );
    assert( dim.y <= AABB::REAL_MAX_DIM );

    events.free();
  }

  void Object::onDestroy()
  {
    synapse.genParts( type->nDebris, p, Vec3::zero(), type->debrisVelocitySpread,
                      type->debrisRejection, type->debrisMass, type->debrisLifeTime,
                      type->debrisColor, type->debrisColorSpread );

    if( !type->onDestroy.isEmpty() ) {
      matrixLua.call( type->onDestroy, this );
    }
  }

  void Object::onDamage( float damage )
  {
    if( !type->onDamage.isEmpty() ) {
      matrixLua.damage = damage;
      matrixLua.call( type->onDamage, this );
    }
  }

  void Object::onHit( const Hit*, float hitMomentum )
  {
    if( !type->onHit.isEmpty() ) {
      matrixLua.hitMomentum = hitMomentum;
      matrixLua.call( type->onHit, this );
    }
  }

  void Object::onUpdate()
  {
    if( !type->onUpdate.isEmpty() ) {
      matrixLua.call( type->onUpdate, this );
    }
  }

  void Object::onUse( Bot *user )
  {
    if( !type->onUse.isEmpty() ) {
      matrixLua.call( type->onUse, this, user );
    }
  }

  void Object::readFull( InputStream *istream )
  {
    p        = istream->readVec3();
    flags    = istream->readInt();
    oldFlags = istream->readInt();
    life     = istream->readFloat();

    int nEvents = istream->readInt();
    for( int i = 0; i < nEvents; i++ ) {
      int id = istream->readInt();
      float intensity = istream->readFloat();

      addEvent( id, intensity );
    }
  }

  void Object::writeFull( OutputStream *ostream ) const
  {
    ostream->writeVec3( p );
    ostream->writeInt( flags );
    ostream->writeInt( oldFlags );
    ostream->writeFloat( life );

    ostream->writeInt( events.length() );
    foreach( event, events.iterator() ) {
      ostream->writeInt( event->id );
      ostream->writeFloat( event->intensity );
    }
  }

  void Object::readUpdate( InputStream *istream )
  {
    life = istream->readFloat();

    int nEvents = istream->readInt();
    for( int i = 0; i < nEvents; i++ ) {
      int   id        = istream->readInt();
      float intensity = istream->readFloat();

      addEvent( id, intensity );
    }
  }

  void Object::writeUpdate( OutputStream *ostream ) const
  {
    ostream->writeFloat( life );

    ostream->writeInt( events.length() );
    foreach( event, events.iterator() ) {
      ostream->writeInt( event->id );
      ostream->writeFloat( event->intensity );
    }
  }

}

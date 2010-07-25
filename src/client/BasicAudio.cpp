/*
 *  BasicAudio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/BasicAudio.hpp"

#include "matrix/Physics.hpp"
#include "client/Camera.hpp"
#include "client/Sound.hpp"

namespace oz
{
namespace client
{

  Pool<BasicAudio> BasicAudio::pool;

  Audio* BasicAudio::create( const Object* obj )
  {
    return new BasicAudio( obj );
  }

  void BasicAudio::play( const Audio* parent )
  {
    const Dynamic* dyn = static_cast<const Dynamic*>( obj );
    const int ( &samples )[ObjectClass::AUDIO_SAMPLES] = obj->type->audioSamples;

    parent = parent == null ? this : parent;

    // friction
    if( ( obj->flags & ( Object::DYNAMIC_BIT | Object::FRICTING_BIT | Object::ON_SLICK_BIT ) ) ==
        ( Object::DYNAMIC_BIT | Object::FRICTING_BIT ) &&
        samples[SND_FRICTING] != -1 && dyn->depth == 0.0f )
    {
      float dv = Math::sqrt( dyn->velocity.x*dyn->velocity.x +
                             dyn->velocity.y*dyn->velocity.y );
      playContSound( samples[SND_FRICTING], dv, uint( size_t( &*dyn ) ), parent->obj );
    }

    // events
    foreach( event, obj->events.citer() ) {
      assert( event->id < ObjectClass::AUDIO_SAMPLES );

      if( event->id >= 0 && samples[event->id] != -1 ) {
        assert( 0.0f <= event->intensity );

        playSound( samples[event->id], event->intensity, parent->obj );
      }
    }
  }

}
}

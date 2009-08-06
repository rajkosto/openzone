/*
 *  Audio.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Audio.h"

#include "Camera.h"
#include "Context.h"
#include "SoundManager.h"

namespace oz
{
namespace client
{

  const float Audio::REFERENCE_DISTANCE = 4.0f;

  void Audio::playSound( int sample, float volume ) const
  {
    uint srcId;

    alGenSources( 1, &srcId );
    alSourcei( srcId, AL_BUFFER, context.sounds[sample].id );
    alSourcef( srcId, AL_REFERENCE_DISTANCE, REFERENCE_DISTANCE );

    // If the object moves since source starts playing and source stands still, it's usually
    // not noticable for short-time source. After all, sound source many times does't move
    // together with the object in many cases (e.g. the sound when an objects hits something).
    //
    // However, when the sound is generated by player (e.g. cries, talk) it is often annoying
    // if the sound source doesn't move with the player. That's why we position the sounds
    // generated by the player at the origin of the coordinate system relative to player.
    if( &*obj == camera.bot ) {
      alSourcei( srcId, AL_SOURCE_RELATIVE, AL_TRUE );
      alSourcefv( srcId, AL_POSITION, Vec3::zero() );
    }
    else {
      alSourcefv( srcId, AL_POSITION, obj->p );
    }
    alSourcef( srcId, AL_GAIN, volume );
    alSourcePlay( srcId );

    soundManager.addSource( srcId );
  }

  void Audio::playContSound( int sample, float volume, uint key ) const
  {
    assert( 0 <= sample && sample < translator.sounds.length() );

    if( soundManager.updateContSource( key ) ) {
      alSourcef( soundManager.getCachedContSourceId(), AL_GAIN, volume );
      alSourcefv( soundManager.getCachedContSourceId(), AL_POSITION, obj->p );
    }
    else {
      uint srcId;

      alGenSources( 1, &srcId );
      alSourcei( srcId, AL_BUFFER, context.sounds[sample].id );
      alSourcei( srcId, AL_LOOPING, AL_TRUE );
      alSourcef( srcId, AL_REFERENCE_DISTANCE, REFERENCE_DISTANCE );
      alSourcefv( srcId, AL_POSITION, obj->p );
      alSourcef( srcId, AL_GAIN, volume );
      alSourcePlay( srcId );

      soundManager.addContSource( key, srcId );
    }
  }

  Audio::Audio( const Object *obj_, const ObjectClass *clazz_ ) : obj( obj_ ), clazz( clazz_ )
  {
    const int *samples = clazz->audioSamples;

    for( int i = 0; i < ObjectClass::AUDIO_SAMPLES; i++ ) {
      if( samples[i] >= 0 ) {
        context.requestSound( samples[i] );
      }
    }
  }

  Audio::~Audio()
  {
    const int *samples = clazz->audioSamples;

    for( int i = 0; i < ObjectClass::AUDIO_SAMPLES; i++ ) {
      if( samples[i] >= 0 ) {
        context.releaseSound( samples[i] );
      }
    }
  }

}
}

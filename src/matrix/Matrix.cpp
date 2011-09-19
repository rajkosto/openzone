/*
 *  Matrix.cpp
 *
 *  World model
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Matrix.hpp"

#include "matrix/Translator.hpp"
#include "matrix/NamePool.hpp"
#include "matrix/Lua.hpp"
#include "matrix/Collider.hpp"
#include "matrix/Physics.hpp"
#include "matrix/Synapse.hpp"
#include "matrix/Vehicle.hpp"
#include "matrix/FloraManager.hpp"

namespace oz
{

  Matrix matrix;

  // default 10000.0f: 100 m/s
  const float Matrix::MAX_VELOCITY2 = 1000000.0f;

  void Matrix::update()
  {
    for( int i = 0; i < orbis.objects.length(); ++i ) {
      Object* obj = orbis.objects[i];

      if( obj != null ) {
        // If this is cleared on the object's update, we may also remove effects that were added
        // by other objects, updated before it.
        obj->events.free();

        // We don't remove objects as they get destroyed but on the next update, so the destroy
        // sound and other effects can be played on an object's destruction.
        if( obj->flags & Object::DESTROYED_BIT ) {
          if( obj->cell == null ) {
            hard_assert( obj->flags & Object::DYNAMIC_BIT );

            synapse.removeCut( static_cast<Dynamic*>( obj ) );
          }
          else {
            synapse.remove( obj );
          }
        }
      }
    }

    for( int i = 0; i < orbis.structs.length(); ++i ) {
      Struct* str = orbis.structs[i];

      if( str == null ) {
        continue;
      }

      str->update();

      if( str->life <= 0.0f ) {
        str->destroy();
      }
    }

    for( int i = 0; i < orbis.objects.length(); ++i ) {
      Object* obj = orbis.objects[i];

      if( obj == null ) {
        continue;
      }

      obj->update();

      // object might have removed itself within onUpdate()
      if( orbis.objects[i] == null ) {
        continue;
      }
      else if( obj->life <= 0.0f ) {
        obj->destroy();
      }
      else if( obj->flags & Object::DYNAMIC_BIT ) {
        Dynamic* dyn = static_cast<Dynamic*>( obj );

        if( dyn->cell == null ) {
          hard_assert( dyn->parent != -1 );

          // remove if its container has been removed
          if( orbis.objects[dyn->parent] == null ) {
            synapse.removeCut( dyn );
          }
        }
        else {
          physics.updateObj( dyn );

          // remove on velocity overflow
          if( dyn->velocity.sqL() > Matrix::MAX_VELOCITY2 ) {
            synapse.remove( obj );
          }
        }
      }
    }

    for( int i = 0; i < orbis.parts.length(); ++i ) {
      Particle* part = orbis.parts[i];

      if( part != null ) {
        part->update();
        physics.updatePart( part );

        if( part->lifeTime <= 0.0f || part->velocity.sqL() > Matrix::MAX_VELOCITY2 ) {
          synapse.remove( part );
        }
      }
    }

    // rotate freeing/waiting/available indices
    orbis.update();
  }

  void Matrix::load( InputStream* istream )
  {
    log.println( "Loading Matrix {" );
    log.indent();

    orbis.terra.load( 0 );
    orbis.load();
    synapse.load();

    if( istream != null ) {
      orbis.read( istream );
    }
    else {
      floraManager.seed();

      lua.staticCall( config.getSet( "matrix.onCreate", "matrix_onCreate" ) );
    }

    log.unindent();
    log.println( "}" );
  }

  void Matrix::unload( OutputStream* ostream )
  {
    log.println( "Unloading Matrix {" );
    log.indent();

    if( ostream != null ) {
      orbis.write( ostream );
    }

    synapse.unload();
    orbis.unload();

    log.unindent();
    log.println( "}" );
  }

  void Matrix::init()
  {
    log.println( "Initialising Matrix {" );
    log.indent();

    namePool.init();
    lua.init();
    orbis.init();

    log.unindent();
    log.println( "}" );
  }

  void Matrix::free()
  {
    log.println( "Freeing Matrix {" );
    log.indent();

    orbis.free();
    lua.free();
    namePool.free();

    log.unindent();
    log.println( "}" );
  }

}

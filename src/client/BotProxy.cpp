/*
 *  BotProxy.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "BotProxy.h"

#include "matrix/BotClass.h"
#include "matrix/VehicleClass.h"
#include "ui/Keyboard.h"
#include "ui/Mouse.h"
#include "Camera.h"

namespace oz
{
namespace client
{

  void BotProxy::begin()
  {
    externalDistFactor = config.getSet( "camera.externalDistFactor", 2.75f );
    isExternal         = false;
    isFreelook         = false;

    bobPhi   = 0.0f;
    bobTheta = 0.0f;
    bobBias  = 0.0f;
  }

  void BotProxy::update()
  {
    if( camera.bot == -1 ) {
      return;
    }

    Bot *bot = static_cast<Bot*>( world.objects[camera.bot] );

    /*
     * Camera
     */
    if( !isExternal || !isFreelook ) {
      bot->h = camera.h;
      bot->v = camera.v;
    }

    bot->actions = 0;

    /*
     * Movement
     */
    if( ui::keyboard.keys[SDLK_w] ) {
      bot->actions |= Bot::ACTION_FORWARD;
    }
    if( ui::keyboard.keys[SDLK_s] ) {
      bot->actions |= Bot::ACTION_BACKWARD;
    }
    if( ui::keyboard.keys[SDLK_d] ) {
      bot->actions |= Bot::ACTION_RIGHT;
    }
    if( ui::keyboard.keys[SDLK_a] ) {
      bot->actions |= Bot::ACTION_LEFT;
    }

    /*
     * Actions
     */
    if( ui::keyboard.keys[SDLK_SPACE] ) {
      bot->actions |= Bot::ACTION_JUMP;
    }
    if( ui::keyboard.keys[SDLK_LCTRL] ) {
      bot->actions |= Bot::ACTION_CROUCH;
    }
    if( ui::keyboard.keys[SDLK_LSHIFT] && !ui::keyboard.oldKeys[SDLK_LSHIFT] ) {
      bot->state ^= Bot::RUNNING_BIT;
    }
    if( ui::keyboard.keys[SDLK_z] ) {
      bot->actions |= Bot::ACTION_EXIT;
    }
    if( ui::keyboard.keys[SDLK_x] ) {
      bot->actions |= Bot::ACTION_EJECT;
    }
    if( camera.isExternal && ui::keyboard.keys[SDLK_LALT] && !ui::keyboard.oldKeys[SDLK_LALT] ) {
      isFreelook = !isFreelook;
    }
    if( ui::keyboard.keys[SDLK_p] && !ui::keyboard.oldKeys[SDLK_p] ) {
      bot->state ^= Bot::STEPPING_BIT;
    }

    bot->state &= ~( Bot::GESTURE0_BIT | Bot::GESTURE1_BIT | Bot::GESTURE2_BIT |
        Bot::GESTURE3_BIT | Bot::GESTURE4_BIT | Bot::GESTURE_ALL_BIT );

    if( ui::keyboard.keys[SDLK_f] ) {
      bot->state |= Bot::GESTURE0_BIT;
    }
    if( ui::keyboard.keys[SDLK_g] ) {
      bot->state |= Bot::GESTURE1_BIT;
    }
    if( ui::keyboard.keys[SDLK_h] ) {
      bot->state |= Bot::GESTURE2_BIT;
    }
    if( ui::keyboard.keys[SDLK_j] ) {
      bot->state |= Bot::GESTURE3_BIT;
    }
    if( ui::keyboard.keys[SDLK_k] ) {
      bot->state |= Bot::GESTURE4_BIT;
    }
    if( ui::keyboard.keys[SDLK_l] ) {
      bot->state |= Bot::GESTURE_ALL_BIT;
    }

    if( ui::keyboard.keys[SDLK_m] && !ui::keyboard.oldKeys[SDLK_m] ) {
      camera.h = bot->h;
      camera.v = bot->v;

      camera.isExternal = !camera.isExternal;
      camera.setState( camera.isExternal ? Camera::EXTERNAL : Camera::INTERNAL );
    }

    if( !ui::mouse.doShow ) {
      if( ui::mouse.buttons & SDL_BUTTON_LMASK ) {
        bot->actions |= Bot::ACTION_ATTACK;
      }
      if( ui::mouse.rightClick ) {
        bot->actions |= Bot::ACTION_USE;
      }
      if( ui::mouse.wheelDown ) {
        bot->actions |= Bot::ACTION_TAKE;
      }
      if( ui::mouse.wheelUp ) {
        bot->actions |= Bot::ACTION_THROW;
      }
      if( ui::mouse.middleClick ) {
        bot->actions |= Bot::ACTION_GRAB;
      }
    }
  }

  void BotProxy::prepare()
  {
    if( camera.bot == -1 ) {
      camera.setState( Camera::FREECAM );
      return;
    }

    const Bot *bot = camera.botObj;

    if( !isExternal ) {
      if( bot->parent != -1 ) {
        Vehicle *veh = static_cast<Vehicle*>( world.objects[bot->parent] );

        assert( veh->flags & Object::VEHICLE_BIT );

        camera.w = 0.0f;
        camera.align();
        camera.warp( bot->p + Vec3( camera.rotMat.y ) * bot->camZ );

        bobPhi   = 0.0f;
        bobTheta = 0.0f;
        bobBias  = 0.0f;
      }
      else {
        const BotClass *clazz = static_cast<const BotClass*>( bot->type );

        if( bot->state & Bot::MOVING_BIT ) {
          if( bot->flags & Object::IN_WATER_BIT ) {
            float bobInc = ( bot->state & Bot::RUNNING_BIT ) && bot->grabObj == -1 ?
              clazz->bobSwimRunInc : clazz->bobSwimInc;

            bobPhi   = Math::mod( bobPhi + bobInc, 360.0f );
            bobTheta = 0.0f;
            bobBias  = Math::sin( Math::rad( -2.0f * bobPhi ) ) * clazz->bobSwimAmplitude;
          }
          else if( ( bot->flags & Object::ON_FLOOR_BIT ) || bot->lower != -1 ) {
            float bobInc =
                ( bot->state & ( Bot::RUNNING_BIT | Bot::CROUCHING_BIT ) ) == Bot::RUNNING_BIT &&
                bot->grabObj == -1 ? clazz->bobRunInc : clazz->bobWalkInc;

            bobPhi   = Math::mod( bobPhi + bobInc, 360.0f );
            bobTheta = Math::sin( Math::rad( bobPhi ) ) * clazz->bobRotation;
            bobBias  = Math::sin( Math::rad( 2.0f * bobPhi ) ) * clazz->bobAmplitude;
          }
        }
        else {
          bobPhi   = 0.0f;
          bobTheta *= BOB_SUPPRESSION_COEF;
          bobBias  *= BOB_SUPPRESSION_COEF;
        }
        if( bot->flags & Object::IN_WATER_BIT ) {
          bobTheta = 0.0f;
        }

        Vec3 p = bot->p;
        p.z += bot->camZ + bobBias;

        camera.w = bobTheta;
        camera.align();
        camera.wrapMoveZ( p );
      }
    }
    else {
      camera.w = 0.0f;
      camera.align();

      float dist;
      if( bot->parent != -1 ) {
        Vehicle *veh = static_cast<Vehicle*>( world.objects[bot->parent] );

        assert( veh->flags & Object::VEHICLE_BIT );

        dist = !veh->dim * externalDistFactor;
      }
      else {
        dist = !bot->dim * externalDistFactor;
      }

      Vec3 origin = bot->p + Vec3( 0.0f, 0.0f, bot->camZ );
      Vec3 offset = -camera.at * dist;

      collider.translate( origin, offset, bot );
      offset *= collider.hit.ratio;
      offset += camera.at * THIRD_PERSON_CLIP_DIST;

      camera.wrapMoveZ( origin + offset );

      bobPhi   = 0.0f;
      bobTheta = 0.0f;
      bobBias  = 0.0f;
    }

    if( bot->grabObj != -1 && world.objects[camera.botObj->grabObj] != null ) {
      camera.setTagged( world.objects[camera.botObj->grabObj] );
    }
    else if( isExternal && isFreelook ) {
      // { hsine, hcosine, vsine, vcosine, vcosine * hsine, vcosine * hcosine }
      float hvsc[6];

      Math::sincos( Math::rad( bot->h ), &hvsc[0], &hvsc[1] );
      Math::sincos( Math::rad( bot->v ), &hvsc[2], &hvsc[3] );

      hvsc[4] = hvsc[3] * hvsc[0];
      hvsc[5] = hvsc[3] * hvsc[1];

      Vec3 at = Vec3( -hvsc[4], hvsc[5], hvsc[2] );

      float distance = static_cast<const BotClass*>( camera.botObj->type )->grabDistance;
      collider.translate( camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ ),
                          at * distance,
                          camera.botObj );

      camera.setTagged( collider.hit.obj );
    }
    else {
      float distance = static_cast<const BotClass*>( camera.botObj->type )->grabDistance;
      collider.translate( camera.botObj->p + Vec3( 0.0f, 0.0f, camera.botObj->camZ ),
                          camera.at * distance,
                          camera.botObj );

      camera.setTagged( collider.hit.obj );
    }
  }

}
}

/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/BotProxy.cc
 */

#include "stable.hh"

#include "client/BotProxy.hh"

#include "client/Camera.hh"

#include "client/ui/UI.hh"
#include "client/ui/GalileoFrame.hh"

namespace oz
{
namespace client
{

const float BotProxy::CAMERA_Z_SMOOTHING       = 0.35f;
const float BotProxy::CAMERA_Z_TOLERANCE       = 0.20f;
const float BotProxy::EXTERNAL_CAM_DIST        = 2.75f;
const float BotProxy::EXTERNAL_CAM_CLIP_DIST   = 0.10f;
const float BotProxy::SHOULDER_CAM_RIGHT       = 0.25f;
const float BotProxy::SHOULDER_CAM_UP          = 0.25f;
const float BotProxy::VEHICLE_CAM_UP_FACTOR    = 0.15f;
const float BotProxy::BOB_SUPPRESSION_COEF     = 0.80f;
const float BotProxy::BINOCULARS_MAGNIFICATION = 0.20f;

BotProxy::BotProxy() :
  hud( null ), infoFrame( null ), inventory( null ), container( null )
{}

void BotProxy::begin()
{
  if( camera.bot == -1 ) {
    return;
  }

  Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

  camera.h = bot->h;
  camera.v = bot->v;
  camera.isExternal = isExternal;
  camera.setTaggedObj( null );
  camera.setTaggedEnt( null );

  hud       = new ui::HudArea();
  infoFrame = new ui::InfoFrame();
  inventory = new ui::InventoryMenu( null );
  container = new ui::InventoryMenu( inventory );

  botEye    = bot->p;
  botEye.z += bot->camZ;
  bobTheta  = 0.0f;
  bobBias   = 0.0f;

  ui::mouse.doShow = false;

  ui::ui.root->add( hud );
  ui::ui.root->add( infoFrame );
  ui::ui.root->add( inventory );
  ui::ui.root->add( container );

  hud->sink();

  infoFrame->show( true );
  inventory->show( false );
  container->show( false );
}

void BotProxy::end()
{
  if( container != null ) {
    ui::ui.root->remove( container );
    container = null;
  }
  if( inventory != null ) {
    ui::ui.root->remove( inventory );
    inventory = null;
  }
  if( infoFrame != null ) {
    ui::ui.root->remove( infoFrame );
    infoFrame = null;
  }
  if( hud != null ) {
    ui::ui.root->remove( hud );
    hud = null;
  }

  ui::mouse.doShow = true;
}

void BotProxy::prepare()
{
  if( camera.bot == -1 ) {
    return;
  }

  const ubyte* keys    = ui::keyboard.keys;
  const ubyte* oldKeys = ui::keyboard.oldKeys;

  bool alt = keys[SDLK_LALT] || keys[SDLK_RALT];

  Bot*     bot = camera.botObj;
  Vehicle* veh = camera.vehicleObj;

  /*
   * Camera
   */

  if( keys[SDLK_q] ) {
    bot->h += camera.keyXSens;
  }
  if( keys[SDLK_e] ) {
    bot->h -= camera.keyXSens;
  }

  if( veh != null ) {
    if( isFreelook ) {
      const VehicleClass* vehClazz = static_cast<const VehicleClass*>( veh->clazz );

      camera.h = angleDiff( camera.h, 0.0f );

      camera.h = clamp( camera.h + camera.relH, vehClazz->lookHMin, vehClazz->lookHMax );
      camera.v = clamp( camera.v + camera.relV, vehClazz->lookVMin, vehClazz->lookVMax );

      camera.h = angleWrap( camera.h );
    }
    else {
      bot->h += camera.relH;
      bot->v += camera.relV;

      camera.h = 0.0f;
      camera.v = Math::TAU / 4.0f;
    }
  }
  else if( isFreelook && isExternal ) {
    camera.h = angleWrap( camera.h + camera.relH );
    camera.v = clamp( camera.v + camera.relV, 0.0f, Math::TAU / 2.0f );
  }
  else {
    bot->h += camera.relH;
    bot->v += camera.relV;

    camera.h = bot->h;
    camera.v = bot->v;
  }

  bot->h = angleWrap( bot->h );
  bot->v = clamp( bot->v, 0.0f, Math::TAU / 2.0f );

  bot->actions = 0;

  /*
   * Movement
   */

  if( keys[SDLK_w] ) {
    bot->actions |= Bot::ACTION_FORWARD;
  }
  if( keys[SDLK_s] ) {
    bot->actions |= Bot::ACTION_BACKWARD;
  }
  if( keys[SDLK_d] ) {
    bot->actions |= Bot::ACTION_RIGHT;
  }
  if( keys[SDLK_a] ) {
    bot->actions |= Bot::ACTION_LEFT;
  }

  /*
   * Actions
   */

  if( keys[SDLK_SPACE] ) {
    bot->actions |= Bot::ACTION_JUMP | Bot::ACTION_VEH_UP;
  }
  if( keys[SDLK_LCTRL] ) {
    bot->actions |= Bot::ACTION_CROUCH | Bot::ACTION_VEH_DOWN;
  }
  if( keys[SDLK_LSHIFT] && !oldKeys[SDLK_LSHIFT] ) {
    bot->state ^= Bot::RUNNING_BIT;
  }

  if( !alt && keys[SDLK_x] && !oldKeys[SDLK_x] ) {
    bot->actions |= Bot::ACTION_EXIT;
  }
  if( alt && keys[SDLK_x] && !oldKeys[SDLK_x] ) {
    bot->actions |= Bot::ACTION_EJECT;
  }
  if( alt && keys[SDLK_k] && !oldKeys[SDLK_k] ) {
    if( bot->hasAttribute( ObjectClass::SUICIDE_BIT ) ) {
      bot->actions |= Bot::ACTION_SUICIDE;
    }
  }

  if( !alt && keys[SDLK_f] ) {
    bot->actions |= Bot::ACTION_POINT;
  }
  if( !alt && keys[SDLK_g] ) {
    bot->actions |= Bot::ACTION_BACK;
  }
  if( !alt && keys[SDLK_h] ) {
    bot->actions |= Bot::ACTION_SALUTE;
  }
  if( !alt && keys[SDLK_j] ) {
    bot->actions |= Bot::ACTION_WAVE;
  }
  if( !alt && keys[SDLK_k] ) {
    bot->actions |= Bot::ACTION_FLIP;
  }

  /*
   * View
   */

  if( !alt && keys[SDLK_n] && !oldKeys[SDLK_n] ) {
    camera.nightVision = !camera.nightVision;
  }
  if( !alt && keys[SDLK_b] && !oldKeys[SDLK_b] ) {
    camera.mag = camera.mag == 1.0f ? BINOCULARS_MAGNIFICATION : 1.0f;
  }
  if( !alt && keys[SDLK_m] && !oldKeys[SDLK_m] ) {
    ui::ui.galileoFrame->setMaximised( !ui::ui.galileoFrame->isMaximised );
  }

  if( camera.nightVision && !bot->hasAttribute( ObjectClass::NIGHT_VISION_BIT ) ) {
    camera.nightVision = false;
  }
  if( camera.mag != 1.0f && !bot->hasAttribute( ObjectClass::BINOCULARS_BIT ) ) {
    camera.mag = 1.0f;
  }

  if( !alt && keys[SDLK_KP_ENTER] && !oldKeys[SDLK_KP_ENTER] ) {
    isExternal = !isExternal;
    camera.isExternal = isExternal;
  }
  if( !alt && keys[SDLK_KP_MULTIPLY] && !oldKeys[SDLK_KP_MULTIPLY] &&
      ( isExternal || veh != null ) )
  {
    isFreelook = !isFreelook;

    if( veh != null ) {
      camera.h = 0.0f;
      camera.v = Math::TAU / 4.0f;
    }
    else {
      camera.h = bot->h;
      camera.v = bot->v;
    }
  }

  /*
   * Mouse
   */

  if( !ui::mouse.doShow ) {
    if( ui::mouse.buttons & SDL_BUTTON_LMASK ) {
      bot->actions |= Bot::ACTION_ATTACK;
    }

    if( ui::mouse.leftClick ) {
      if( bot->cargo != -1 ) {
        bot->rotateCargo();
      }
    }
    if( ui::mouse.rightClick ) {
      if( bot->parent != -1 ) {
        bot->actions |= Bot::ACTION_VEH_NEXT_WEAPON;
      }
      else if( camera.entityObj != null ) {
        bot->trigger( camera.entityObj );
      }
      else if( camera.objectObj != null ) {
        bot->use( camera.objectObj );
      }
    }
    else if( ui::mouse.middleClick ) {
      if( bot->cargo != -1 ) {
        bot->grab();
      }
      else if( camera.entity != -1 ) {
        bot->lock( camera.entityObj );
      }
      else if( camera.object != -1 ) {
        Dynamic* dyn = static_cast<Dynamic*>( const_cast<Object*>( camera.objectObj ) );

        if( dyn->flags & Object::DYNAMIC_BIT ) {
          bot->grab( dyn );
        }
      }
    }
    else if( ui::mouse.wheelDown ) {
      if( camera.objectObj != null ) {
        if( camera.objectObj->flags & Object::BROWSABLE_BIT ) {
          ui::mouse.doShow = true;
        }
        else {
          Dynamic* dyn = static_cast<Dynamic*>( const_cast<Object*>( camera.objectObj ) );

          if( dyn->flags & Object::DYNAMIC_BIT ) {
            bot->take( dyn );
          }
        }
      }
    }
    else if( ui::mouse.wheelUp ) {
      if( bot->cargo != -1 ) {
        bot->throwCargo();
      }
    }
  }

  /*
   * Other
   */

  if( !alt && keys[SDLK_i] && !oldKeys[SDLK_i] ) {
    if( camera.allowReincarnation ) {
      bot->actions = 0;
      camera.setBot( null );
      return;
    }
  }

  if( !alt && keys[SDLK_o] ) {
    if( keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT] ) {
      orbis.caelum.time -= 0.1f * Timer::TICK_TIME * orbis.caelum.period;
    }
    else {
      orbis.caelum.time += 0.1f * Timer::TICK_TIME * orbis.caelum.period;
    }
  }

  if( keys[SDLK_TAB] && !oldKeys[SDLK_TAB] ) {
    ui::mouse.doShow = !ui::mouse.doShow;
  }
}

void BotProxy::update()
{
  if( camera.bot == -1 ) {
    camera.setState( Camera::STRATEGIC );
    return;
  }

  const Bot*      bot      = camera.botObj;
  const BotClass* botClazz = static_cast<const BotClass*>( bot->clazz );
  const Vehicle*  veh      = camera.vehicleObj;

  if( veh != null ) {
    botEye = bot->p + Mat44::rotation( veh->rot ).z * bot->camZ;
  }
  else {
    botEye.x = bot->p.x;
    botEye.y = bot->p.y;

    float actualZ = bot->p.z + bot->camZ;

    botEye.z = Math::mix( botEye.z, actualZ, CAMERA_Z_SMOOTHING );
    botEye.z = clamp( botEye.z, actualZ - CAMERA_Z_TOLERANCE, actualZ + CAMERA_Z_TOLERANCE );
  }

  if( !isExternal ) {
    if( veh != null ) { // inside vehicle
      bobTheta = 0.0f;
      bobBias  = 0.0f;

      camera.w = 0.0f;
      camera.move( botEye );
      camera.align();
    }
    else { // 1st person, not in vehicle
      if( ( bot->state & ( Bot::MOVING_BIT | Bot::SWIMMING_BIT | Bot::CLIMBING_BIT ) ) ==
          Bot::MOVING_BIT )
      {
        float phase = bot->step * Math::TAU;
        float sine  = Math::sin( phase );

        bobTheta = sine * botClazz->bobRotation;
        bobBias  = sine*sine * botClazz->bobAmplitude;
      }
      else if( ( bot->state & ( Bot::MOVING_BIT | Bot::SWIMMING_BIT | Bot::CLIMBING_BIT ) ) ==
               ( Bot::MOVING_BIT | Bot::SWIMMING_BIT ) )
      {
        float sine = Math::sin( bot->step * Math::TAU / 2.0f );

        bobTheta = 0.0f;
        bobBias  = sine*sine * botClazz->bobSwimAmplitude;
      }
      else {
        bobTheta *= BOB_SUPPRESSION_COEF;
        bobBias  *= BOB_SUPPRESSION_COEF;
      }

      camera.h = bot->h;
      camera.v = bot->v;
      camera.w = bobTheta;
      camera.move( Point( botEye.x, botEye.y, botEye.z + bobBias ) );
      camera.align();
    }
  }
  else { // external
    bobTheta = 0.0f;
    bobBias  = 0.0f;

    if( !isFreelook ) {
      camera.h = bot->h;
      camera.v = bot->v;
    }

    camera.w = 0.0f;
    camera.align();

    Vec3 offset;

    if( veh != null ) {
      float dist = veh->dim.fastL() * EXTERNAL_CAM_DIST;
      offset = camera.rotMat * Vec3( 0.0f, VEHICLE_CAM_UP_FACTOR * dist, dist );
    }
    else {
      float dist = bot->dim.fastL() * EXTERNAL_CAM_DIST;
      offset = camera.rotMat * Vec3( SHOULDER_CAM_RIGHT, SHOULDER_CAM_UP, dist );
    }

    collider.translate( botEye, offset, bot );
    offset *= collider.hit.ratio;

    float dist = !offset;
    if( dist > EXTERNAL_CAM_CLIP_DIST ) {
      offset *= ( dist - EXTERNAL_CAM_CLIP_DIST ) / dist;
    }
    else {
      offset = Vec3::ZERO;
    }

    camera.move( botEye + offset );
  }

  if( bot->parent != -1 ) {
    camera.setTaggedObj( null );
  }
  else if( bot->cargo != -1 ) {
    camera.setTaggedObj( orbis.objects[bot->cargo] );
  }
  else {
    // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
    float hvsc[6];

    Math::sincos( bot->h, &hvsc[0], &hvsc[1] );
    Math::sincos( bot->v, &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[2] * hvsc[0];
    hvsc[5] = hvsc[2] * hvsc[1];

    Vec3  at    = Vec3( -hvsc[4], +hvsc[5], -hvsc[3] );
    Point eye   = bot->p + Vec3( 0.0f, 0.0f, bot->camZ );
    Vec3  reach = at * botClazz->reachDist;

    collider.mask = ~0;
    collider.translate( eye, reach, bot );
    collider.mask = Object::SOLID_BIT;

    camera.setTaggedObj( collider.hit.obj );
    camera.setTaggedEnt( collider.hit.entity );
  }
}

void BotProxy::reset()
{
  isExternal = false;
  isFreelook = false;
}

void BotProxy::read( InputStream* istream )
{
  bobTheta = 0.0f;
  bobBias  = 0.0f;

  isExternal = istream->readBool();
  isFreelook = istream->readBool();
}

void BotProxy::write( BufferStream* ostream ) const
{
  ostream->writeBool( isExternal );
  ostream->writeBool( isFreelook );
}

}
}

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
 * @file client/ui/HudArea.cc
 */

#include <client/ui/HudArea.hh>

#include <matrix/Physics.hh>
#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/Camera.hh>

namespace oz
{
namespace client
{
namespace ui
{

const float HudArea::VEHICLE_DIM      = VEHICLE_SIZE / 2.0f;
const float HudArea::CROSS_FADE_COEFF = 8.0f;

void HudArea::drawBotCrosshair()
{
  const Bot*      me      = camera.botObj;
  const BotClass* myClazz = static_cast<const BotClass*>( camera.botObj->clazz );

  float delta  = max( 1.0f - abs( camera.unit.headRot.w ), 0.0f );
  float alpha  = 1.0f - CROSS_FADE_COEFF * Math::sqrt( delta );
  float life   = max( 2.0f * me->life / myClazz->life - 1.0f, 0.0f );
  Vec4  colour = Math::mix( Vec4( 1.00f, 0.50f, 0.25f, alpha ),
                            Vec4( 1.00f, 1.00f, 1.00f, alpha ),
                            life );

  shape.colour( colour );
  glBindTexture( GL_TEXTURE_2D, crossTex.id() );
  shape.fill( crossIconX, crossIconY, ICON_SIZE, ICON_SIZE );
  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

  if( me->parent < 0 && ( camera.object >= 0 || camera.entity >= 0 ) ) {
    const Object*      obj      = camera.objectObj;
    const ObjectClass* objClazz = obj == nullptr ? nullptr : obj->clazz;
    const Dynamic*     dyn      = static_cast<const Dynamic*>( obj );
    const Bot*         bot      = static_cast<const Bot*>( obj );
    const Entity*      ent      = camera.entityObj;
    const EntityClass* entClazz = ent == nullptr ? nullptr : ent->clazz;

    // it might happen that bot itself is tagged object for a frame when switching from freecam
    // into a bot
    if( obj == camera.botObj ) {
      return;
    }

    if( ent != nullptr ) {
      if( lastEntityId != camera.entity ) {
        lastEntityId = camera.entity;

        title.set( descTextX, descTextY, "%s", entClazz->title.cstr() );
      }

      title.draw( this );

      shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );

      if( entClazz->target >= 0 && ent->key >= 0 ) {
        glBindTexture( GL_TEXTURE_2D, useTex.id() );
        shape.fill( rightIconX, rightIconY, ICON_SIZE, ICON_SIZE );
      }

      if( ent->key < 0 ) {
        glBindTexture( GL_TEXTURE_2D, lockedTex.id() );
        shape.fill( bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE );
      }
      else if( ent->key > 0 ) {
        glBindTexture( GL_TEXTURE_2D, unlockedTex.id() );
        shape.fill( bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE );
      }
    }
    else {
      if( obj->flags & Object::BOT_BIT ) {
        life = max( 2.0f * obj->life / objClazz->life - 1.0f, 0.0f );
      }
      else {
        life = obj->life / objClazz->life;
      }

      float status = obj->status();

      taggedLife.draw( this, healthBarX, healthBarY + 14, ICON_SIZE + 16, 10, life );
      if( status >= 0.0f ) {
        taggedStatus.draw( this, healthBarX, healthBarY + 7, ICON_SIZE + 16, 8, status );
      }

      if( lastObjectId != camera.object ) {
        lastObjectId = camera.object;

        String sTitle = ( obj->flags & Object::BOT_BIT ) && !bot->name.isEmpty() ?
                        bot->name + " (" + objClazz->title + ")" : objClazz->title;

        title.set( descTextX, descTextY, "%s", sTitle.cstr() );
      }

      title.draw( this );

      shape.colour( 1.0f, 1.0f, 1.0f, 1.0f );

      if( obj->flags & Object::BROWSABLE_BIT ) {
        glBindTexture( GL_TEXTURE_2D, browseTex.id() );
        shape.fill( leftIconX, leftIconY, ICON_SIZE, ICON_SIZE );
      }
      if( ( obj->flags & Object::USE_FUNC_BIT ) &&
          !( obj->flags & ( Object::WEAPON_BIT | Object::VEHICLE_BIT ) ) )
      {

        glBindTexture( GL_TEXTURE_2D,
                       obj->flags & Object::USE_FUNC_BIT ? useTex.id() : deviceTex.id() );
        shape.fill( rightIconX, rightIconY, ICON_SIZE, ICON_SIZE );
      }

      if( !( obj->flags & Object::SOLID_BIT ) ) {
        glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
        return;
      }

      if( obj->flags & Object::VEHICLE_BIT ) {
        const Vehicle* vehicle = static_cast<const Vehicle*>( obj );

        if( vehicle->pilot < 0 ) {
          glBindTexture( GL_TEXTURE_2D, mountTex.id() );
          shape.fill( rightIconX, rightIconY, ICON_SIZE, ICON_SIZE );
        }
      }
      else if( obj->flags & Object::WEAPON_BIT ) {
        if( me->canEquip( static_cast<const Weapon*>( obj ) ) ) {
          glBindTexture( GL_TEXTURE_2D, equipTex.id() );
          shape.fill( rightIconX, rightIconY, ICON_SIZE, ICON_SIZE );
        }
      }

      if( obj->flags & Object::ITEM_BIT ) {
        glBindTexture( GL_TEXTURE_2D, takeTex.id() );
        shape.fill( leftIconX, leftIconY, ICON_SIZE, ICON_SIZE );
      }

      if( me->cargo < 0 && me->weapon < 0 &&
          ( obj->flags & Object::DYNAMIC_BIT ) &&
            abs( dyn->mass * physics.gravity ) <= myClazz->grabWeight &&
            // not climbing or on a ladder
            !( me->state & ( Bot::LADDER_BIT | Bot::LEDGE_BIT ) ) &&
            // if it is not a bot that is holding something
            ( !( obj->flags & Object::BOT_BIT ) || bot->cargo < 0 ) )
      {
        float dimX = bot->dim.x + dyn->dim.x;
        float dimY = bot->dim.y + dyn->dim.y;
        float dist = Math::sqrt( dimX*dimX + dimY*dimY ) + Bot::GRAB_EPSILON;

        if( dist <= myClazz->reachDist ) {
          glBindTexture( GL_TEXTURE_2D, liftTex.id() );
          shape.fill( bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE );
        }
      }
      if( camera.botObj->cargo >= 0 ) {
        glBindTexture( GL_TEXTURE_2D, grabTex.id() );
        shape.fill( bottomIconX, bottomIconY, ICON_SIZE, ICON_SIZE );
      }
    }
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }
}

void HudArea::drawBotStatus()
{
  const Bot*      bot      = camera.botObj;
  const BotClass* botClazz = static_cast<const BotClass*>( camera.botObj->clazz );

  float life    = 2.0f * bot->life / botClazz->life - 1.0f;
  float stamina = bot->stamina / botClazz->stamina;

  botLife.draw( this, life );
  botStamina.draw( this, stamina );

  const Weapon* weaponObj = static_cast<const Weapon*>( orbis.obj( bot->weapon ) );

  if( weaponObj != nullptr ) {
    Pair<int> pos = align( style.botWeapon.x, style.botWeapon.y,
                           style.botWeapon.w, style.botWeapon.h );

    shape.colour( style.colours.frame );
    shape.fill( pos.x, pos.y, style.botWeapon.w, style.botWeapon.h );

    weaponName.setPosition( pos.x + 4, pos.y + 2 );
    weaponRounds.setPosition( pos.x + style.botWeapon.w - 4, pos.y + 2 );

    if( lastWeaponId != bot->weapon ) {
      lastWeaponId = bot->weapon;
      weaponName.setText( "%s", weaponObj->clazz->title.cstr() );
    }
    if( lastWeaponRounds != weaponObj->nRounds ) {
      lastWeaponRounds = weaponObj->nRounds;

      if( weaponObj->nRounds < 0 ) {
        weaponRounds.setText( "∞" );
      }
      else {
        weaponRounds.setText( "%d", weaponObj->nRounds );
      }
    }

    weaponName.draw( this );
    weaponRounds.draw( this );
  }
}

void HudArea::drawVehicleStatus()
{
  if( orbis.obj( camera.botObj->parent ) == nullptr ) {
    vehicleModel->setModel( -1 );
    return;
  }

  const Bot*          bot      = camera.botObj;
  const Vehicle*      vehicle  = static_cast<const Vehicle*>( orbis.obj( bot->parent ) );
  const VehicleClass* vehClazz = static_cast<const VehicleClass*>( vehicle->clazz );

  // HACK close gap between vehicle model and weapons.
  int modelBiasY = ( 4 - vehClazz->nWeapons ) * ( style.vehicleWeapon[0].h + 4 );

  vehicleModel->setModel( vehClazz->imagoModel );
  vehicleModel->reposition();
  vehicleModel->y -= modelBiasY;

  float hull = max( vehicle->life / vehClazz->life, 0.0f );
  float fuel = max( vehicle->fuel / vehClazz->fuel, 0.0f );

  vehicleHull.draw( this, hull );
  vehicleFuel.draw( this, fuel );

  for( int i = 0; i < vehClazz->nWeapons; ++i ) {
    int    labelIndex  = vehClazz->nWeapons - i - 1;
    Label& nameLabel   = vehicleWeaponNames[labelIndex];
    Label& roundsLabel = vehicleWeaponRounds[labelIndex];
    const Style::Area& areaStyle = style.vehicleWeapon[labelIndex];

    Pair<int> pos = align( areaStyle.x, areaStyle.y, areaStyle.w, areaStyle.h );

    if( i == vehicle->weapon ) {
      shape.colour( style.colours.frame );
      shape.fill( pos.x, pos.y, areaStyle.w, areaStyle.h );
    }

    nameLabel.setPosition( pos.x + 2, pos.y + 2 );
    roundsLabel.setPosition( pos.x + areaStyle.w - 4, pos.y + 2 );

    if( lastVehicleId != bot->parent ) {
      nameLabel.setText( "%s", vehClazz->weaponTitles[i].cstr() );
    }
    if( lastVehicleWeaponRounds[labelIndex] != vehicle->nRounds[i] ) {
      lastVehicleWeaponRounds[labelIndex] = vehicle->nRounds[i];

      if( vehicle->nRounds[i] < 0 ) {
        roundsLabel.setText( "∞" );
      }
      else {
        roundsLabel.setText( "%d", vehicle->nRounds[i] );
      }
    }

    nameLabel.draw( this );
    roundsLabel.draw( this );
  }

  lastVehicleId = bot->parent;
}

void HudArea::onReposition()
{
  width        = camera.width;
  height       = camera.height;

  crossIconX   = ( width - ICON_SIZE ) / 2;
  crossIconY   = ( height - ICON_SIZE ) / 2;
  leftIconX    = crossIconX - ICON_SIZE;
  leftIconY    = crossIconY;
  rightIconX   = crossIconX + ICON_SIZE;
  rightIconY   = crossIconY;
  bottomIconX  = crossIconX;
  bottomIconY  = crossIconY - ICON_SIZE;
  healthBarX   = crossIconX - 8;
  healthBarY   = crossIconY + ICON_SIZE;
  descTextX    = width / 2;
  descTextY    = crossIconY + ICON_SIZE + 36;

  lastObjectId = -1;
  lastEntityId = -1;

  title.set( descTextX, descTextY, " " );
}

void HudArea::onVisibilityChange( bool )
{
  lastObjectId  = -1;
  lastEntityId  = -1;
  lastWeaponId  = -1;
  lastVehicleId = -1;
}

void HudArea::onUpdate()
{
  const Bot* bot = camera.botObj;

  // we need this is onUpdate() rather than in onDraw() for the rare case if an object is replaced
  // by a new one with the same id (onDraw() may not be called each frame and may miss this switch)
  if( camera.object != lastObjectId ) {
    lastObjectId = -1;
  }
  if( camera.entity != lastEntityId ) {
    lastEntityId = -1;
  }
  if( camera.state != Camera::UNIT || camera.bot < 0 ) {
    lastWeaponId = -1;
  }
  else if( orbis.obj( bot->parent ) == nullptr ) {
    lastVehicleId = -1;
  }
}

bool HudArea::onMouseEvent()
{
  return Area::passMouseEvents();
}

void HudArea::onDraw()
{
  if( camera.botObj == nullptr || ( camera.botObj->state & Bot::DEAD_BIT ) ) {
    return;
  }

  drawBotCrosshair();
  drawBotStatus();
  drawVehicleStatus();
  drawChildren();
}

HudArea::HudArea() :
  Area( camera.width, camera.height ),
  title( 0, 0, ALIGN_CENTRE, Font::LARGE, " " ),
  weaponName( 0, 0, ALIGN_LEFT, Font::LARGE, " " ),
  weaponRounds( 0, 0, ALIGN_RIGHT, Font::LARGE, "∞" ),
  taggedLife( &style.taggedLife ),
  taggedStatus( &style.taggedStatus ),
  botLife( &style.botLife ),
  botStamina( &style.botStamina ),
  vehicleHull( &style.vehicleHull ),
  vehicleFuel( &style.vehicleFuel ),
  vehicleModel( nullptr ),
  lastObjectId( -1 ),
  lastEntityId( -1 ),
  lastWeaponId( -1 ),
  lastWeaponRounds( -1 ),
  lastVehicleId( -1 )
{
  flags |= UPDATE_BIT | PINNED_BIT;

  for( int i = 0; i < Vehicle::MAX_WEAPONS; ++i ) {
    lastVehicleWeaponRounds[i] = -1;
    vehicleWeaponNames[i]      = Label( 0, 0, ALIGN_LEFT, Font::LARGE, " " );
    vehicleWeaponRounds[i]     = Label( 0, 0, ALIGN_RIGHT, Font::LARGE, "∞" );
  }

  vehicleModel = new ModelField( nullptr, style.vehicleField.w, style.vehicleField.h );
  add( vehicleModel, style.vehicleField.x, style.vehicleField.y );

  crossTex.load( "@ui/icon/crosshair.dds" );
  useTex.load( "@ui/icon/use.dds" );
  deviceTex.load( "@ui/icon/device.dds" );
  equipTex.load( "@ui/icon/equip.dds" );
  mountTex.load( "@ui/icon/mount.dds" );
  takeTex.load( "@ui/icon/take.dds" );
  browseTex.load( "@ui/icon/browse.dds" );
  liftTex.load( "@ui/icon/lift.dds" );
  grabTex.load( "@ui/icon/grab.dds" );
  lockedTex.load( "@ui/icon/locked.dds" );
  unlockedTex.load( "@ui/icon/unlocked.dds" );
}

}
}
}

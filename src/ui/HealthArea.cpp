/*
 *  HealthArea.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "HealthArea.h"

#include "matrix/BotClass.h"
#include "client/Camera.h"

namespace oz
{
namespace client
{
namespace ui
{

  void HealthArea::draw()
  {
    if( camera.bot != null ) {
      glColor4f( 1.0f, 1.0f, 1.0f, 0.6f );
      rect( 0, 30, 250, 20 );
      rect( 0,  0, 250, 20 );

      float damage       = camera.bot->damage / ( (BotClass*) camera.bot->type )->damage;
      int   damageWidth  = max( (int) ( damage * 248.0f ), 0 );
      float stamina      = camera.bot->stamina / ( (BotClass*) camera.bot->type )->stamina;
      int   staminaWidth = max( (int) ( stamina * 248.0f ), 0 );

      glColor4f( 1.0f - damage, damage, 0.0f, 0.6f );
      fill( 1, 31, damageWidth, 18 );
      glColor4f( 0.7f - 0.7f * stamina, 0.3f, 0.5f + 0.5f * stamina, 0.6f );
      fill( 1, 1, staminaWidth, 18 );
    }
  }

}
}
}
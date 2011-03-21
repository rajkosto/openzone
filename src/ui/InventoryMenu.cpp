/*
 *  InventoryMenu.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/InventoryMenu.hpp"

#include "client/Camera.hpp"
#include "client/Context.hpp"
#include "client/Shape.hpp"
#include "client/Colours.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  const float InventoryMenu::SLOT_DIMF = float( InventoryMenu::SLOT_SIZE ) / 2.0f;

  InventoryMenu::InventoryMenu() :
      Frame( 5, 5, SLOT_SIZE * COLS, SLOT_SIZE * ROWS + HEADER_SIZE + FOOTER_SIZE )
  {
    useTexId = context.loadTexture( "ui/use.ozcTex" );
    tagged = -1;
    row = 0;
  }

  InventoryMenu::~InventoryMenu()
  {
    glDeleteTextures( 1, &useTexId );
  }

  bool InventoryMenu::onMouseEvent()
  {
    if( camera.state != Camera::BOT || camera.bot == -1 || !mouse.doShow ) {
      return false;
    }

    Frame::onMouseEvent();

    Bot* bot = static_cast<Bot*>( orbis.objects[camera.bot] );

    tagged = -1;

    int minY = y + FOOTER_SIZE;
    int maxY = y + FOOTER_SIZE + ROWS * SLOT_SIZE;

    // mouse-over selects
    if( minY <= mouse.y && mouse.y < maxY ) {
      int i = ( mouse.x - x ) / SLOT_SIZE + COLS * ( ROWS - 1 - ( mouse.y - minY ) / SLOT_SIZE );

      if( 0 <= i && i < COLS * ROWS ) {
        tagged = row * COLS + i;

        if( mouse.rightClick ) {
          bot->taggedItem = tagged;
          bot->actions |= Bot::ACTION_INV_USE;
        }
        else if( mouse.middleClick ) {
          bot->taggedItem = tagged;
          bot->actions |= Bot::ACTION_INV_GRAB;

          mouse.doShow = false;
        }
      }
    }

    // scroll
    if( mouse.wheelDown ) {
      ++row;
    }
    if( mouse.wheelUp ) {
      --row;
    }

    // works fine if there's no items (then nRows == 1)
    int nRows = ( bot->items.length() - 1 ) / COLS + 1;
    row = ( row + nRows ) % nRows;

    return true;
  }

  void InventoryMenu::onDraw()
  {
    if( camera.state != Camera::BOT || camera.bot == -1 || !mouse.doShow ) {
      tagged = -1;
      row = 0;
      return;
    }

    Frame::onDraw();

    setFont( Font::TITLE );
    printCentred( SLOT_SIZE * COLS / 2, -HEADER_SIZE / 2, "Inventory" );

    // set shaders
    for( int i = 0; i < translator.shaders.length(); ++i ) {
      if( i == shader.ui ) {
        continue;
      }

      shader.use( i );

      tf.applyCamera();

      shader.setAmbientLight( Colours::WHITE );
      shader.setSkyLight( Vec3( 0.0f, 0.0f, 1.0f ), Colours::BLACK );
      shader.updateLights();

      glUniform1f( param.oz_Fog_start, 1000000.0f );
      glUniform1f( param.oz_Fog_end, 2000000.0f );
    }

    glDisable( GL_BLEND );

    tf.model = Mat44::ID;
    tf.model.translate( Vec3( float( x + SLOT_SIZE / 2 ), float( y + SLOT_SIZE / 2 + FOOTER_SIZE ), 0.0f ) );
    tf.model.translate( Vec3( float( COLS * SLOT_SIZE ), float( ROWS * SLOT_SIZE ), 0.0f ) );
    tf.model.z = Vec4::ZERO;

    const Vector<int>& items = camera.botObj->items;

    int minIndex = row * COLS;
    int maxIndex = min( minIndex + COLS * ROWS, items.length() );
    Dynamic* taggedItem = null;

    for( int i = minIndex; i < maxIndex; ++i ) {
      Dynamic* item = static_cast<Dynamic*>( orbis.objects[items[i]] );

      hard_assert( ( item->flags & Object::DYNAMIC_BIT ) && ( item->flags & Object::ITEM_BIT ) );

      float size = item->dim.fastL();
      float scale = SLOT_DIMF / size;

      if( i % COLS == 0 ) {
        tf.model.translate( Vec3( -COLS * SLOT_SIZE, -SLOT_SIZE, 0.0f ) );
      }

      tf.push();
      tf.model.rotateX( Math::rad( -60.0f ) );
      tf.model.rotateZ( Math::rad( +70.0f ) );
      tf.model.scale( Vec3( scale, scale, scale ) );

      if( i == tagged ) {
        shader.colour = Colours::TAG;
        taggedItem = item;
      }

      context.drawModel( item, null );

      if( i == tagged ) {
        shader.colour = Colours::WHITE;
      }

      tf.pop();
      tf.model.translate( Vec3( float( SLOT_SIZE ), 0.0f, 0.0f ) );
    }

    shape.bindVertexArray();

    shader.use( shader.ui );
    glEnable( GL_BLEND );

    tagged = -1;

    if( taggedItem != null ) {
      float life = taggedItem->life / taggedItem->clazz->life;
      int lifeWidth = int( life * 46.0f );

      hard_assert( 0.0f <= life && life <= 1.0f );

      glUniform4f( param.oz_Colour, 1.0f - life, life, 0.0f, 0.6f );
      fill( -51, -15, lifeWidth, 10 );

      glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 0.6f );
      rect( -52, -16, 48, 12 );

      if( taggedItem->flags & Object::USE_FUNC_BIT ) {
        glUniform1i( param.oz_IsTextureEnabled, true );
        glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
        glBindTexture( GL_TEXTURE_2D, useTexId );

        shape.fill( x + width - ICON_SIZE - 4, y + 4, ICON_SIZE, ICON_SIZE );

        glUniform1i( param.oz_IsTextureEnabled, false );
      }

      setFont( Font::SANS );
      printBaseline( 4, FOOTER_SIZE / 2, "%s", taggedItem->clazz->description.cstr() );
    }
  }

}
}
}

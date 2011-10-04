/*
 *  UI.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/UI.hpp"

#include "client/Camera.hpp"
#include "client/Colours.hpp"
#include "client/Context.hpp"
#include "client/Shape.hpp"

#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  UI ui;

  UI::UI() : root( null ), loadingScreen( null ), hudArea( null ), strategicArea( null ),
      inventoryMenu( null ), browseMenu( null ), infoFrame( null ), galileoFrame( null ),
      musicPlayer( null ), buildMenu( null ), debugFrame( null )
  {}

  void UI::showLoadingScreen( bool doShow )
  {
    root->focus( loadingScreen );
    loadingScreen->show( doShow );
  }

  void UI::update()
  {
    if( keyboard.keys[SDLK_TAB] & ~keyboard.oldKeys[SDLK_TAB] ) {
      mouse.doShow = !mouse.doShow;
    }
    if( mouse.doShow != isFreelook ) {
      isFreelook = mouse.doShow;

      foreach( area, root->children.iter() ) {
        if( !( area->flags & Area::PINNED_BIT ) ) {
          area->show( isFreelook );
        }
      }
    }
    if( isFreelook ) {
      root->passMouseEvents();
    }
    Area::update();
  }

  void UI::draw()
  {
    OZ_GL_CHECK_ERROR();

    tf.ortho();
    tf.camera = Mat44::ID;

    // set shaders
    for( int i = 0; i < library.shaders.length(); ++i ) {
      if( shader.isLoaded || i == shader.ui ) {
        shader.use( i );

        tf.applyCamera();

        shader.setAmbientLight( Vec4( 0.6f, 0.5f, 0.6f, 1.0f ) );
        shader.setCaelumLight( Vec3( 0.67f, -0.67f, -0.33f ), Vec4( 0.6f, 0.6f, 0.6f, 1.0f ) );
        shader.updateLights();

        glUniform1f( param.oz_Fog_start, 1000000.0f );
        glUniform1f( param.oz_Fog_end, 2000000.0f );
      }
    }

    shader.use( shader.ui );
    glBindTexture( GL_TEXTURE_2D, 0 );

    shape.bindVertexArray();

    glClear( GL_DEPTH_BUFFER_BIT );
    glEnable( GL_BLEND );

    root->drawChildren();
    mouse.draw();

    glDisable( GL_BLEND );

    OZ_GL_CHECK_ERROR();
  }

  void UI::load()
  {
    isFreelook = false;

    try {
      strategicArea = new StrategicArea();
      hudArea       = new HudArea();
      inventoryMenu = new InventoryMenu( null );
      browseMenu    = new InventoryMenu( inventoryMenu );
      infoFrame     = new InfoFrame();
      galileoFrame  = new GalileoFrame();
      musicPlayer   = new MusicPlayer();
      buildMenu     = showBuild ? new BuildMenu() : null;
      debugFrame    = showDebug ? new DebugFrame() : null;
    }
    catch( ... ) {
      delete strategicArea;
      delete hudArea;
      delete inventoryMenu;
      delete browseMenu;
      delete infoFrame;
      delete galileoFrame;
      delete musicPlayer;
      delete buildMenu;
      delete debugFrame;

      strategicArea = null;
      hudArea       = null;
      inventoryMenu = null;
      browseMenu    = null;
      infoFrame     = null;
      galileoFrame  = null;
      musicPlayer   = null;
      buildMenu     = null;
      debugFrame    = null;

      throw;
    }

    root->add( strategicArea );
    root->add( hudArea );
    root->add( inventoryMenu );
    root->add( browseMenu );
    root->add( infoFrame );
    root->add( galileoFrame );
    root->add( musicPlayer );

    if( showBuild ) {
      root->add( buildMenu );
    }
    if( showDebug ) {
      root->add( debugFrame );
    }

    root->focus( loadingScreen );

    isFreelook = mouse.doShow;

    foreach( area, root->children.iter() ) {
      if( !( area->flags & Area::PINNED_BIT ) ) {
        area->show( isFreelook );
      }
    }
  }

  void UI::unload()
  {
    if( debugFrame != null ) {
      root->remove( debugFrame );
      debugFrame = null;
    }
    if( buildMenu != null ) {
      root->remove( buildMenu );
      buildMenu = null;
    }
    if( musicPlayer != null ) {
      root->remove( musicPlayer );
      musicPlayer = null;
    }
    if( infoFrame != null ) {
      root->remove( infoFrame );
      infoFrame = null;
    }
    if( galileoFrame != null ) {
      root->remove( galileoFrame );
      galileoFrame = null;
    }
    if( browseMenu != null ) {
      root->remove( browseMenu );
      browseMenu = null;
    }
    if( inventoryMenu != null ) {
      root->remove( inventoryMenu );
      inventoryMenu = null;
    }
    if( hudArea != null ) {
      root->remove( hudArea  );
      hudArea = null;
    }
    if( strategicArea != null ) {
      root->remove( strategicArea );
      strategicArea = null;
    }
  }

  void UI::init()
  {
    mouse.init();

    if( !font.init() ) {
      throw Exception( "Failed to load font" );
    }

    isFreelook = false;

    showBuild = config.getSet( "ui.showBuild", false );
    showDebug = config.getSet( "ui.showDebug", false );

    mouse.load();

    root = new Area( camera.width, camera.height );
    loadingScreen = new LoadingArea();

    root->add( loadingScreen );

    hard_assert( ui::ui.strategicArea == null );
  }

  void UI::free()
  {
    delete root;

    mouse.unload();

    Area::updateAreas.clear();
    Area::updateAreas.dealloc();

    font.free();
    mouse.free();
  }

}
}
}

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
 * @file client/Client.cc
 *
 * Game initialisation and main loop.
 */

#include "stable.hh"

#include "client/Client.hh"

#include "BuildInfo.hh"

#include "client/Context.hh"
#include "client/Camera.hh"
#include "client/MenuStage.hh"
#include "client/GameStage.hh"
#include "client/Sound.hh"
#include "client/Render.hh"
#include "client/Loader.hh"

#include <cerrno>
#include <unistd.h>

#ifdef _WIN32
# undef WIN32_LEAN_AND_MEAN
# include <shlobj.h>
#endif

namespace oz
{
namespace client
{

Client client;

void Client::shutdown()
{
  if( initFlags & INIT_STAGE_INIT ) {
    gameStage.free();
    menuStage.free();
  }
  if( initFlags & INIT_AUDIO ) {
    sound.free();
  }
  if( initFlags & INIT_RENDER ) {
    render.free();
  }
  if( initFlags & INIT_CONTEXT ) {
    context.free();
  }
  if( initFlags & INIT_LIBRARY ) {
    library.free();
  }
  if( initFlags & INIT_LINGUA ) {
    lingua.free();
  }

  if( ( initFlags & ( INIT_CONFIG | INIT_MAIN_LOOP ) ) == INIT_MAIN_LOOP ) {
    String configDir = config.get( "dir.config", "" );

    if( !configDir.isEmpty() ) {
      File configFile( configDir + "/client.rc" );

      config.exclude( "dir.config" );
      config.exclude( "dir.local" );
      config.removeUnused();

#ifdef _WIN32
      config.save( configFile, "\r\n" );
#else
      config.save( configFile );
#endif
    }
  }

  config.clear( initFlags & INIT_CONFIG );

  if( initFlags & INIT_PHYSFS ) {
    PhysFile::free();
  }
  if( initFlags & INIT_SDL ) {
    SDL_Quit();
  }

  if( initFlags & INIT_MAIN_LOOP ) {
    Alloc::printStatistics();

    log.print( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " finished on " );
    log.printTime();
    log.printEnd();
  }
}

void Client::printUsage()
{
  log.println( "Usage:" );
  log.indent();
  log.println( "openzone [-v] [-l | -i <mission>] [-t <num>] [-p <prefix>]" );
  log.println();
  log.println( "-v" );
  log.println( "\tMore verbose log output." );
  log.println();
  log.println( "-l" );
  log.println( "\tSkip main menu and load the last autosaved state." );
  log.println();
  log.println( "-i <mission>" );
  log.println( "\tSkip main menu and start mission <mission>." );
  log.println( "\tMissions reside in <prefix>/share/openzone/lua/mission in files named" );
  log.println( "\t<mission>.lua files." );
  log.println();
  log.println( "-t <num>" );
  log.println( "\tExit after <num> seconds (can be a floating-point number) and use 42 as" );
  log.println( "\tthe random seed. Useful for benchmarking." );
  log.println();
  log.println( "-p <prefix>" );
  log.println( "\tSets data directory to <prefix>/share/openzone." );
  log.println( "\tDefault: '%s'.", OZ_INSTALL_PREFIX );
  log.println();
  log.unindent();
}

int Client::main( int argc, char** argv )
{
  initFlags = 0;

  bool   doAutoload    = false;
  bool   isBenchmark   = false;
  float  benchmarkTime = 0.0f;
  String mission;

  optind = 1;
  int opt;
  while( ( opt = getopt( argc, argv, "vli:t:p:" ) ) != -1 ) {
    switch( opt ) {
      case 'v': {
        log.isVerbose = true;
        break;
      }
      case 'l': {
        doAutoload = true;
        break;
      }
      case 'i': {
        mission = optarg;
        break;
      }
      case 't': {
        errno = 0;
        char* end;
        benchmarkTime = strtof( optarg, &end );

        if( errno != 0 ) {
          printUsage();
          return EXIT_FAILURE;
        }

        config.add( "seed", "42" );
        isBenchmark = true;
        break;
      }
      case 'p': {
        config.add( "dir.prefix", optarg );
        break;
      }
      default: {
        log.println();
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

#ifdef _WIN32

  char configRoot[MAX_PATH];
  char localRoot[MAX_PATH];

  if( !SHGetSpecialFolderPath( null, configRoot, CSIDL_APPDATA, false ) ) {
    throw Exception( "Failed to access APPDATA directory" );
  }
  if( !SHGetSpecialFolderPath( null, localRoot, CSIDL_LOCAL_APPDATA, false ) ) {
    throw Exception( "Failed to access LOCAL_APPDATA directory" );
  }

  File configDir( String::str( "%s\\" OZ_APPLICATION_NAME, configRoot ) );
  File localDir( String::str( "%s\\" OZ_APPLICATION_NAME, localRoot ) );

#else

  const char* home       = SDL_getenv( "HOME" );
  const char* configRoot = SDL_getenv( "XDG_CONFIG_HOME" );
  const char* localRoot  = SDL_getenv( "XDG_LOCAL_HOME" );

  if( home == null ) {
    throw Exception( "Cannot determine user home directory from environment" );
  }

  File configDir = configRoot == null ?
      File( String::str( "%s/.config/" OZ_APPLICATION_NAME, home ) ) :
      File( String::str( "%s/" OZ_APPLICATION_NAME, configRoot ) );

  File localDir = localRoot == null ?
      File( String::str( "%s/.local/share/" OZ_APPLICATION_NAME, home ) ) :
      File( String::str( "%s/" OZ_APPLICATION_NAME, localRoot ) );

#endif

  String dir = configDir.path();
  if( File::mkdir( dir, 0755 ) ) {
    log.println( "Profile directory '%s' created", dir.cstr() );
  }

  dir = configDir.path() + "/saves";
  if( File::mkdir( dir, 0755 ) ) {
    log.println( "Directory for saved games '%s' created", dir.cstr() );
  }

  dir = configDir.path() + "/screenshots";
  if( File::mkdir( dir, 0755 ) ) {
    log.println( "Directory for screenshots '%s' created", dir.cstr() );
  }

  dir = localDir.path();
  if( File::mkdir( dir, 0755 ) ) {
    log.println( "Directory for per-user content '%s' created", dir.cstr() );
  }

  String logPath = configDir.path() + "/client.log";

  if( !log.init( logPath, true ) ) {
    throw Exception( "Can't create/open log file '%s' for writing\n", logPath.cstr() );
  }

  log.println( "Log file '%s'", logPath.cstr() );

  log.print( OZ_APPLICATION_TITLE " " OZ_APPLICATION_VERSION " started on " );
  log.printTime();
  log.printEnd();

  if( SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO ) != 0 ) {
    throw Exception( "Failed to initialise SDL" );
  }
  initFlags |= INIT_SDL;

  if( !PhysFile::init() ) {
    throw Exception( "PhysicsFS initialisation failed" );
  }
  initFlags |= INIT_PHYSFS;

  log.verboseMode = true;
  log.println( "Build details {" );
  log.indent();
  log.println( "Date:            %s", BuildInfo::TIME );
  log.println( "Host system:     %s", BuildInfo::HOST_SYSTEM );
  log.println( "Target system:   %s", BuildInfo::TARGET_SYSTEM );
  log.println( "Build type:      %s", BuildInfo::BUILD_TYPE );
  log.println( "Compiler:        %s", BuildInfo::COMPILER );
  log.println( "Compiler flags:  %s", BuildInfo::CXX_FLAGS );
  log.println( "Linker flags:    %s", BuildInfo::EXE_LINKER_FLAGS );
  log.unindent();
  log.println( "}" );
  log.verboseMode = false;

  File configFile( configDir.path() + "/client.rc" );

  if( config.load( configFile ) ) {
    log.printEnd( "Configuration read from '%s'", configFile.path().cstr() );

    if( String::equals( config.get( "_version", "" ), OZ_APPLICATION_VERSION ) ) {
      initFlags |= INIT_CONFIG;
    }
    else {
      log.println( "Invalid configuration file version, configuration will be cleaned and written "
                   "on exit" );

      config.add( "_version", OZ_APPLICATION_VERSION );
      config.get( "_version", "" );
    }
  }
  else {
    log.println( "No configuration file, default configuration will be used and written on exit" );
    config.add( "_version", OZ_APPLICATION_VERSION );
    config.get( "_version", "" );
  }

  config.add( "dir.config", configDir.path() );
  config.add( "dir.local", localDir.path() );

  // tag variables as used
  config.get( "dir.config", "" );
  config.get( "dir.local", "" );

  String prefix = config.getSet( "dir.prefix", OZ_INSTALL_PREFIX );
  File dataDir( prefix + "/share/" OZ_APPLICATION_NAME );

  if( String::equals( config.getSet( "seed", "TIME" ), "TIME" ) ) {
    int seed = int( Time::clock() );
    Math::seed( seed );
    log.println( "Random generator seed set to the current time: %d", seed );
  }
  else {
    int seed = config.get( "seed", 0 );
    Math::seed( seed );
    log.println( "Random generator seed set to: %d", seed );
  }

  // Don't mess with screensaver. In X11 it only makes effect for windowed mode, in fullscreen
  // mode screensaver never starts anyway. Turning off screensaver has a side effect: if the game
  // crashes, it remains turned off. Besides that, in X11 several programs (e.g. IM clients like
  // Pidgin, Kopete, Psi) rely on screensaver's counter, so they don't detect that you are away
  // if the screensaver is screwed.
  if( !config.getSet( "screen.disableScreensaver", false ) ) {
    SDL_putenv( const_cast<char*>( "SDL_VIDEO_ALLOW_SCREENSAVER=1" ) );
  }
  if( config.getSet( "screen.vsync", true ) ) {
    SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
  }
  ui::keyboard.init();

  log.println( "Content search path {" );
  log.indent();

  const char* userMusicPath = config.getSet( "dir.music", "" );

  if( !String::isEmpty( userMusicPath ) ) {
    if( !PhysFile::mount( userMusicPath, "/music", true ) ) {
      throw Exception( "Failed to mount '%s' on /music in PhysicsFS", userMusicPath );
    }
    log.println( "%s [mounted on /music]", userMusicPath );
  }

  if( PhysFile::mount( localDir.path(), null, true ) ) {
    log.println( "%s", localDir.path().cstr() );

    DArray<File> list = localDir.ls();

    foreach( file, list.citer() ) {
      if( file->hasExtension( "zip" ) || file->hasExtension( "7z" ) ) {
        if( !PhysFile::mount( file->path(), null, true ) ) {
          throw Exception( "Failed to mount '%s' on / in PhysicsFS", file->path().cstr() );
        }
        log.println( "%s", file->path().cstr() );
      }
    }
  }

  if( PhysFile::mount( dataDir.path(), null, true ) ) {
    log.println( "%s", dataDir.path().cstr() );

    DArray<File> list = dataDir.ls();

    foreach( file, list.citer() ) {
      if( file->hasExtension( "zip" ) || file->hasExtension( "7z" ) ) {
        if( !PhysFile::mount( file->path(), null, true ) ) {
          throw Exception( "Failed to mount '%s' on / in PhysicsFS", file->path().cstr() );
        }
        log.println( "%s", file->path().cstr() );
      }
    }
  }

  log.unindent();
  log.println( "}" );

  const char* locale = config.getSet( "lingua", "en" );

  log.print( "Setting localisation '%s' ...", locale );
  if( lingua.init( locale ) ) {
    log.printEnd( " OK" );
    initFlags |= INIT_LINGUA;
  }
  else {
    log.printEnd( " Failed" );
  }

  initFlags |= INIT_LIBRARY;
  library.init();

  initFlags |= INIT_CONTEXT;
  context.init();

  initFlags |= INIT_RENDER;
  render.init();

  initFlags |= INIT_AUDIO;
  sound.init();

  initFlags |= INIT_STAGE_INIT;
  menuStage.init();
  gameStage.init();

  Stage::nextStage = null;

  if( !mission.isEmpty() ) {
    gameStage.mission = mission;
    stage = &gameStage;
  }
  else if( doAutoload ) {
    gameStage.stateFile = GameStage::AUTOSAVE_FILE;
    stage = &gameStage;
  }
  else {
    stage = &menuStage;
  }

  stage->load();

  SDL_Event event;

  // set mouse cursor to centre of the screen and clear any events (key presses and mouse moves)
  // from before
  ushort screenCentreX = ushort( camera.centreX );
  ushort screenCentreY = ushort( camera.centreY );

  SDL_WarpMouse( screenCentreX, screenCentreY );
  SDL_PumpEvents();
  while( SDL_PollEvent( &event ) );

  bool isAlive        = true;
  bool isActive       = true;

  // time passed form start of the frame
  uint timeSpent;
  uint timeNow;
  uint timeZero       = Time::clock();
  // time at start of the frame
  uint timeLast       = timeZero;
  uint timeLastRender = timeZero;

  initFlags |= INIT_MAIN_LOOP;

  log.println( "Main loop {" );
  log.indent();

  // THE MAGNIFICENT MAIN LOOP
  do {
    // read input & events
    ui::keyboard.prepare();
    ui::mouse.prepare();

    while( SDL_PollEvent( &event ) != 0 ) {
      switch( event.type ) {
        case SDL_KEYDOWN: {
          const SDL_keysym& keysym = event.key.keysym;

          ui::keyboard.keys[keysym.sym] |= SDL_PRESSED;

          if( keysym.sym == SDLK_F10 ) {
            if( keysym.mod == 0 ) {
              loader.makeScreenshot();
            }
          }
          else if( keysym.sym == SDLK_F11 ) {
            if( keysym.mod == 0 ) {
              if( render.toggleFullscreen() ) {
                ui::mouse.isGrabbed = !ui::mouse.isGrabbed;
                ui::mouse.isJailed = true;

                SDL_ShowCursor( false );
              }
            }
            else if( ( keysym.mod & KMOD_CTRL ) && !( keysym.mod & ~KMOD_CTRL ) ) {
              if( !ui::mouse.isGrabbed ) {
                ui::mouse.isJailed = !ui::mouse.isJailed;

                SDL_ShowCursor( !ui::mouse.isJailed );
              }
            }
          }
          else if( keysym.sym == SDLK_F12 ) {
            if( keysym.mod == 0 ) {
              SDL_WM_IconifyWindow();
            }
            else if( ( keysym.mod & KMOD_CTRL ) && !( keysym.mod & ~KMOD_CTRL ) ) {
              isAlive = false;
            }
          }
          break;
        }
        case SDL_MOUSEBUTTONUP: {
          ui::mouse.currButtons &= char( ~SDL_BUTTON( event.button.button ) );
          break;
        }
        case SDL_MOUSEBUTTONDOWN: {
          ui::mouse.buttons |= char( SDL_BUTTON( event.button.button ) );
          ui::mouse.currButtons |= char( SDL_BUTTON( event.button.button ) );

          if( ui::mouse.buttons & SDL_BUTTON_WUMASK ) {
            ++ui::mouse.relZ;
          }
          if( ui::mouse.buttons & SDL_BUTTON_WDMASK ) {
            --ui::mouse.relZ;
          }
          break;
        }
        case SDL_ACTIVEEVENT: {
          if( event.active.state & SDL_APPACTIVE ) {
            if( event.active.gain ) {
              sound.resume();
              isActive = true;
            }
            else {
              sound.suspend();
              isActive = false;
            }
          }
          break;
        }
        case SDL_QUIT: {
          isAlive = false;
          break;
        }
      }
    }

    // Update mouse motion.
    int x, y;

    SDL_PumpEvents();
    SDL_GetRelativeMouseState( &x, &y );

    ui::mouse.relX = +x;
    ui::mouse.relY = -y;
    ui::mouse.update();

    // Centre mouse cursor and suppress mouse motion event.
    if( ui::mouse.isJailed ) {
      SDL_WarpMouse( screenCentreX, screenCentreY );
      SDL_PumpEvents();
      SDL_GetRelativeMouseState( &x, &y );
    }

    // Waste time when iconified.
    if( !isActive ) {
      SDL_Delay( Timer::TICK_MILLIS );

      timeSpent = Time::clock() - timeLast;
      timeLast += timeSpent;

      continue;
    }

    timer.tick();

    isAlive &= stage->update();

    if( Stage::nextStage != null ) {
      stage->unload();

      stage = Stage::nextStage;
      Stage::nextStage = null;

      stage->load();

      timeLast = Time::clock();
      continue;
    }

    timeNow = Time::clock();
    timeSpent = timeNow - timeLast;

    // Skip rendering graphics, only play sounds if there's not enough time.
    if( timeSpent >= uint( Timer::TICK_MILLIS ) && timeNow - timeLastRender < 1000 ) {
      stage->present( false );
    }
    else {
      stage->present( true );

      timer.frame();
      // If there's still some time left, sleep.
      timeLastRender = Time::clock();
      timeSpent = timeLastRender - timeLast;

      if( timeSpent < uint( Timer::TICK_MILLIS ) ) {
        SDL_Delay( Timer::TICK_MILLIS - timeSpent );

        timeSpent = Timer::TICK_MILLIS;
      }
    }

    if( timeSpent > 100 ) {
      timer.drop( timeSpent - Timer::TICK_MILLIS );
      timeLast += timeSpent - Timer::TICK_MILLIS;
    }
    timeLast += Timer::TICK_MILLIS;

    if( isBenchmark && float( Time::clock() - timeZero ) >= benchmarkTime * 1000.0f ) {
      isAlive = false;
    }
  }
  while( isAlive );

  log.unindent();
  log.println( "}" );

  stage->unload();

  return EXIT_SUCCESS;
}

}
}

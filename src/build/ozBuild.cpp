/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file build/ozBuild.cpp
 *
 * Data builder.
 */

#include "stable.hpp"

#include "Build.hpp"

#include "matrix/Library.hpp"

#include "client/Render.hpp"
#include "client/Module.hpp"
#include "client/OpenGL.hpp"

#include "build/Context.hpp"
#include "build/Compiler.hpp"
#include "build/Mouse.hpp"
#include "build/Caelum.hpp"
#include "build/Terra.hpp"
#include "build/BSP.hpp"
#include "build/OBJ.hpp"
#include "build/MD2.hpp"
#include "build/MD3.hpp"

#include "build/modules/GalileoModule.hpp"

#include <unistd.h>

#include <SDL/SDL_main.h>

bool oz::Alloc::isLocked = true;

namespace oz
{
namespace build
{

static const char* const CREATE_DIRS[] = {
  "bsp",
  "caelum",
  "class",
  "glsl",
  "lua",
  "lua/matrix",
  "lua/mission",
  "lua/nirvana",
  "mdl",
  "music",
  "name",
  "snd",
  "terra",
  "ui",
  "ui/cur",
  "ui/font",
  "ui/icon"
};

static void printUsage()
{
  log.println( "Usage:" );
  log.indent();
  log.println( "ozBuild [OPTIONS] <prefix>" );
  log.println();
  log.println( "<prefix>" );
  log.println( "\tSets data directory to <prefix>/share/openzone." );
  log.println();
  log.println( "-u" );
  log.println( "\tBuild UI." );
  log.println();
  log.println( "-t" );
  log.println( "\tBuild terrae (terrains)." );
  log.println();
  log.println( "-c" );
  log.println( "\tBuild caela (skies)." );
  log.println();
  log.println( "-b" );
  log.println( "\tCompile maps into BSPs and build BPSs with referenced textures." );
  log.println();
  log.println( "-m" );
  log.println( "\tBuild models." );
  log.println();
  log.println( "-o" );
  log.println( "\tBuild modules." );
  log.println();
  log.println( "-l" );
  log.println( "\tCheck syntax of Lua scripts." );
  log.println();
  log.println( "-A" );
  log.println( "\tBuild everything." );
  log.println();
  log.println( "-C" );
  log.println( "\tUse S3 texture compression" );
  log.println();
  log.unindent();
}

static void createDirs()
{
  log.println( "Creating directory structure {" );
  log.indent();

  for( int i = 0; i < aLength( CREATE_DIRS ); ++i ) {
    log.print( "%-11s ...", CREATE_DIRS[i] );

    File dir( CREATE_DIRS[i] );
    if( dir.getType() == File::DIRECTORY ) {
      log.printEnd( " OK, exists" );
      continue;
    }

    if( !File::mkdir( CREATE_DIRS[i] ) ) {
      log.printEnd( " Failed" );
      throw Exception( "Failed to create directories" );
    }

    log.printEnd( " OK, created" );
  }

  log.unindent();
  log.println( "}" );
}

static void buildTextures( const char* srcDir, const char* destDir,
                           bool wrap, int magFilter, int minFilter )
{
  log.println( "Building textures in '%s' {", srcDir );
  log.indent();

  String sSrcDir = srcDir;
  String sDestDir = destDir;
  File dir( sSrcDir );
  DArray<File> dirList = dir.ls();

  sSrcDir  = sSrcDir + "/";
  sDestDir = sDestDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "png" ) && !file->hasExtension( "jpg" ) ) {
      continue;
    }

    String srcPath = file->path();
    String destPath = sDestDir + file->baseName() + ".ozcTex";

    log.println( "Building texture '%s' {", srcPath.cstr() );
    log.indent();

    uint id = Context::loadRawTexture( srcPath, wrap, magFilter, minFilter );

    hard_assert( id != 0 );

    BufferStream os;

    log.println( "Compiling into '%s'", destPath.cstr() );
    Context::writeTexture( id, &os );

    glDeleteTextures( 1, &id );

    if( !File( destPath ).write( &os ) ) {
      throw Exception( "Texture writing failed" );
    }

    log.unindent();
    log.println( "}" );
  }

  log.unindent();
  log.println( "}" );
}

static void buildTerrae()
{
  log.println( "Building Terrae {" );
  log.indent();

  String srcDir = "terra";
  File dir( srcDir );
  DArray<File> dirList = dir.ls();

  srcDir = srcDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    Terra::build( file->baseName() );
  }

  log.unindent();
  log.println( "}" );
}

static void buildCaela()
{
  log.println( "Building Caela {" );
  log.indent();

  String srcDir = "caelum";
  File dir( srcDir );
  DArray<File> dirList = dir.ls();

  srcDir = srcDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    Caelum::build( name );
  }

  log.unindent();
  log.println( "}" );
}

static void compileBSPs()
{
  log.println( "Compiling BSPs {" );
  log.indent();

  String dirName = "data/maps";
  File dir( dirName );
  DArray<File> dirList = dir.ls();

  dirName = dirName + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "map" ) ) {
      continue;
    }

    String baseName = file->baseName();
    const char* dot = String::findLast( baseName, '.' );

    if( baseName.equals( "autosave" ) ||
        ( dot != null && String::equals( dot + 1, "autosave" ) ) )
    {
      continue;
    }

    String cmdLine = "q3map2 -fs_basepath . -fs_game data " + String( file->path() );

    log.println( "%s", cmdLine.cstr() );
    log.println();
    log.println( "========== q3map2 OUTPUT BEGIN %s ==========", baseName.cstr() );
    log.println();
    if( system( cmdLine ) != 0 ) {
      throw Exception( "BSP map compilation failed" );
    }
    log.println();
    log.println( "========== q3map2 OUTPUT END %s ==========", baseName.cstr() );
    log.println();
  }

  log.unindent();
  log.println( "}" );
}

static void buildBSPs()
{
  log.println( "Building BSPs {" );
  log.indent();

  String srcDir = "data/maps";
  String destDir = "bsp";
  File dir( srcDir );
  DArray<File> dirList = dir.ls();

  srcDir = srcDir + "/";
  destDir = destDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    BSP::build( file->baseName() );
  }

  log.unindent();
  log.println( "}" );
}

static void buildBSPTextures()
{
  log.println( "Building BSP textures {" );
  log.indent();

  for( int i = 0; i < library.textures.length(); ++i ) {
    if( !BSP::usedTextures.get( i ) ) {
      continue;
    }

    String srcPath = library.textures[i].path;
    String destPath = "bsp/" + library.textures[i].name + ".ozcTex";

    log.println( "Building texture '%s' {", srcPath.cstr() );
    log.indent();

    uint id = Context::loadRawTexture( srcPath );

    hard_assert( id != 0 );

    BufferStream os;

    log.println( "Compiling into '%s'", destPath.cstr() );
    Context::writeTexture( id, &os );

    int slash = destPath.lastIndex( '/' );
    hard_assert( slash != -1 );
    String dir = destPath.substring( 0, slash );

    File::mkdir( dir );

    if( !File( destPath ).write( &os ) ) {
      throw Exception( "Texture writing failed" );
    }

    log.unindent();
    log.println( "}" );
  }

  BSP::usedTextures.dealloc();

  log.unindent();
  log.println( "}" );
}

static void buildModels()
{
  log.println( "Building models {" );
  log.indent();

  String dirName = "mdl";
  File dir( dirName );
  DArray<File> dirList = dir.ls();

  dirName = dirName + "/";

  foreach( file, dirList.citer() ) {
    String name = file->name();
    String path = file->path();

    if( File( path + "/data.obj" ).getType() != File::MISSING ) {
      OBJ::build( path );
    }
    else if( File( path + "/tris.md2" ).getType() != File::MISSING ) {
      MD2::build( path );
    }
    else if( File( path + "/.md3" ).getType() != File::MISSING ) {
      MD3::build( path );
    }
  }

  log.unindent();
  log.println( "}" );
}

static void buildModules()
{
  log.println( "Building Modules {" );
  log.indent();

  GalileoModule::build();

  log.unindent();
  log.println( "}" );
}

static void checkLua( const char* path )
{
  log.println( "Checking Lua scripts '%s' {", path );
  log.indent();

  String srcDir = path + String( "/" );
  File dir( path );
  DArray<File> dirList = dir.ls();

  String sources;

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "lua" ) ) {
      continue;
    }

    sources = sources + " " + file->path();
  }

  log.println( "luac -p%s", sources.cstr() );
  if( system( "luac -p" + sources ) != 0 ) {
    throw Exception( "Lua syntax check failed" );
  }

  log.unindent();
  log.println( "}" );
}

static void shutdown()
{
  compiler.free();
  client::render.free( true );
  library.free();
  config.clear( true );

  SDL_Quit();

  Alloc::printStatistics();

  log.print( OZ_APPLICATION_TITLE " Build finished on " );
  log.printTime();
  log.printEnd();
}

int main( int argc, char** argv )
{
  bool doUI      = false;
  bool doTerrae  = false;
  bool doCaela   = false;
  bool doBSPs    = false;
  bool doModels  = false;
  bool doModules = false;
  bool doLua     = false;

  optind = 1;
  int opt;
  while( ( opt = getopt( argc, argv, "utcbmdlCA" ) ) != -1 ) {
    switch( opt ) {
      case 'u': {
        doUI = true;
        break;
      }
      case 't': {
        doTerrae = true;
        break;
      }
      case 'c': {
        doCaela = true;
        break;
      }
      case 'b': {
        doBSPs = true;
        break;
      }
      case 'm': {
        doModels = true;
        break;
      }
      case 'd': {
        doModels = true;
        break;
      }
      case 'l': {
        doLua = true;
        break;
      }
      case 'C': {
        Context::useS3TC = true;
        break;
      }
      case 'A': {
        doUI      = true;
        doTerrae  = true;
        doCaela   = true;
        doBSPs    = true;
        doModels  = true;
        doModules = true;
        doLua     = true;
        break;
      }
      default: {
        log.println();
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

  if( optind < argc ) {
    config.add( "dir.prefix", argv[optind] );
  }
  else {
    printUsage();
    return EXIT_FAILURE;
  }

  log.print( OZ_APPLICATION_TITLE " Build started on " );
  log.printTime();
  log.printEnd();

  log.println( "Build details {" );
  log.indent();

  log.println( "Date:            %s", Build::TIME );
  log.println( "Host system:     %s", Build::HOST_SYSTEM );
  log.println( "Target system:   %s", Build::TARGET_SYSTEM );
  log.println( "Build type:      %s", Build::BUILD_TYPE );
  log.println( "Compiler:        %s", Build::COMPILER );
  log.println( "Compiler flags:  %s", Build::CXX_FLAGS );
  log.println( "Linker flags:    %s", Build::EXE_LINKER_FLAGS );

  log.unindent();
  log.println( "}" );

  String prefixDir = config.get( "dir.prefix", OZ_INSTALL_PREFIX );
  String dataDir   = prefixDir + "/share/" OZ_APPLICATION_NAME;

  log.print( "Setting working directory to data directory '%s' ...", dataDir.cstr() );
  if( !File::chdir( dataDir ) ) {
    log.printEnd( " Failed" );
    return EXIT_FAILURE;
  }
  log.printEnd( " OK" );

  SDL_Init( SDL_INIT_VIDEO );

  uint startTime = SDL_GetTicks();

  library.buildInit();

  config.add( "screen.width", "400" );
  config.add( "screen.height", "40" );
  config.add( "screen.bpp", "32" );
  config.add( "screen.full", "false" );
  client::render.init( true );
  SDL_WM_SetCaption( OZ_APPLICATION_TITLE " :: Building data ...", null );

  if( !client::shader.hasS3TC && Context::useS3TC ) {
    throw Exception( "S3 texture compression enabled but not supported" );
  }

  createDirs();

  if( doUI ) {
    bool useS3TC = Context::useS3TC;
    Context::useS3TC = false;

    Mouse::build();
    buildTextures( "ui/icon", "ui/icon", true, GL_LINEAR, GL_LINEAR );

    Context::useS3TC = useS3TC;
  }

  if( doTerrae ) {
    buildTerrae();
  }

  if( doCaela ) {
    buildCaela();
  }

  if( doBSPs ) {
    compileBSPs();
    buildBSPs();
    buildBSPTextures();
  }

  if( doModels ) {
    buildModels();
  }

  if( doModules ) {
    buildModules();
  }

  if( doLua ) {
    checkLua( "lua/matrix" );
    checkLua( "lua/nirvana" );
    checkLua( "lua/mission" );
  }

  uint endTime = SDL_GetTicks();

  log.println( "Build time: %.2f s", float( endTime - startTime ) / 1000.0f );

  return EXIT_SUCCESS;
}

}
}

int main( int argc, char** argv )
{
  oz::System::catchSignals();
#ifndef NDEBUG
  oz::System::enableHalt( true );
#endif

  oz::Alloc::isLocked = false;

  int exitCode = EXIT_FAILURE;

  printf( "OpenZone  Copyright (C) 2002-2011  Davorin Učakar\n"
      "This program comes with ABSOLUTELY NO WARRANTY.\n"
      "This is free software, and you are welcome to redistribute it\n"
      "under certain conditions; See COPYING file for details.\n\n" );

  try {
    exitCode = oz::build::main( argc, argv );
    oz::build::shutdown();
  }
  catch( const oz::Exception& e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.printException( e );
    oz::log.println();

    if( oz::log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s\n", e.what() );
      fprintf( stderr, "  in %s\n\n", e.function );
      fprintf( stderr, "  at %s:%d\n\n", e.file, e.line );
    }

    oz::System::resetSignals();
    abort();
  }
  catch( const std::exception& e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.println();
    oz::log.println( "EXCEPTION: %s", e.what() );
    oz::log.println();

    if( oz::log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s\n\n", e.what() );
    }

    oz::System::resetSignals();
    abort();
  }

//   oz::Alloc::isLocked = true;
  oz::Alloc::printLeaks();
  return exitCode;
}

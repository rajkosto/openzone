/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Library.cpp
 *
 * Map of all resources, object types, scripts etc.
 */

#include "stable.hpp"

#include "matrix/Library.hpp"

#include "matrix/Vehicle.hpp"

#include <dirent.h>
#include <sys/types.h>

#define OZ_REGISTER_BASECLASS( name ) \
  baseClasses.include( #name, name##Class::createClass )

namespace oz
{
namespace matrix
{

Library library;

Library::Resource::Resource( const String& name_, const String& path_ ) :
    name( name_ ), path( path_ )
{}

const ObjectClass* Library::objClass( const char* name ) const
{
  const ObjectClass* const* value = objClasses.find( name );
  if( value == null ) {
    throw Exception( "Invalid object class requested '" + String( name ) + "'" );
  }
  else {
    return *value;
  }
}

int Library::textureIndex( const char* name ) const
{
  const int* value = textureIndices.find( name );
  if( value == null ) {
    throw Exception( "Invalid texture requested '" + String( name ) + "'" );
  }
  else {
    return *value;
  }
}

int Library::soundIndex( const char* name ) const
{
  const int* value = soundIndices.find( name );
  if( value == null ) {
    throw Exception( "Invalid sound requested '" + String( name ) + "'" );
  }
  else {
    return *value;
  }
}

int Library::shaderIndex( const char* name ) const
{
  const int* value = shaderIndices.find( name );
  if( value == null ) {
    throw Exception( "Invalid shader requested '" + String( name ) + "'" );
  }
  else {
    return *value;
  }
}

int Library::terraIndex( const char* name ) const
{
  const int* value = terraIndices.find( name );
  if( value == null ) {
    throw Exception( "Invalid terra index requested '" + String( name ) + "'" );
  }
  else {
    return *value;
  }
}

int Library::caelumIndex( const char* name ) const
{
  const int* value = caelumIndices.find( name );
  if( value == null ) {
    throw Exception( "Invalid caelum index requested '" + String( name ) + "'" );
  }
  else {
    return *value;
  }
}

int Library::bspIndex( const char* name ) const
{
  const int* value = bspIndices.find( name );
  if( value == null ) {
    throw Exception( "Invalid BSP index requested '" + String( name ) + "'" );
  }
  else {
    return *value;
  }
}

int Library::modelIndex( const char* name ) const
{
  const int* value = modelIndices.find( name );
  if( value == null ) {
    throw Exception( "Invalid model index requested '" + String( name ) + "'" );
  }
  else {
    return *value;
  }
}

int Library::nameListIndex( const char* name ) const
{
  const int* value = nameListIndices.find( name );
  if( value == null ) {
    throw Exception( "Invalid name list index requested '" + String( name ) + "'" );
  }
  else {
    return *value;
  }
}

int Library::fragPoolIndex( const char* name ) const
{
  const int* value = fragPoolIndices.find( name );
  if( value == null ) {
    throw Exception( "Invalid name list index requested '" + String( name ) + "'" );
  }
  else {
    return *value;
  }
}

int Library::deviceIndex( const char* name ) const
{
  const int* value = deviceIndices.find( name );

  return value == null ? -1 : *value;
}

int Library::imagoIndex( const char* name ) const
{
  const int* value = imagoIndices.find( name );

  return value == null ? -1 : *value;
}

int Library::audioIndex( const char* name ) const
{
  const int* value = audioIndices.find( name );

  return value == null ? -1 : *value;
}

void Library::initShaders()
{
  File dir;
  DArray<File> dirList;

  log.println( "shaders (*.vert/*.frag in 'glsl') {" );
  log.indent();

  dir.setPath( "glsl" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'glsl'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "vert" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    shaderIndices.add( name, shaders.length() );
    shaders.add( Resource( name, "" ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
}

void Library::initTextures()
{
  File dir;
  File subDir;
  DArray<File> dirList;
  DArray<File> subDirList;

  log.println( "textures (*.ozcTex in 'bsp/*') {" );
  log.indent();

  dir.setPath( "bsp" );

  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'bsp'" );
    log.unindent();
    log.println( "}" );

    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    subDir.setPath( file->path() );
    if( !subDir.ls( &subDirList ) ) {
      free();

      log.println( "Cannot open directory '%s'", subDir.path() );
      log.unindent();
      log.println( "}" );

      throw Exception( "Library initialisation failure" );
    }

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "ozcTex" ) ) {
        continue;
      }

      String name = subDir.name() + String( "/" ) + file->baseName();

      log.println( "%s", name.cstr() );

      textureIndices.add( name, textures.length() );
      textures.add( Resource( name, file->path() ) );
    }
    subDirList.dealloc();
  }

  log.unindent();
  log.println( "}" );
}

void Library::initBuildTextures()
{
  File dir;
  File subDir;
  DArray<File> dirList;
  DArray<File> subDirList;

  log.println( "textures (*.png, *.jpeg, *.jpg in 'data/textures/*') {" );
  log.indent();

  dir.setPath( "data/textures" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'data/textures'" );
    log.unindent();
    log.println( "}" );

    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    subDir.setPath( file->path() );
    if( !subDir.ls( &subDirList ) ) {
      free();

      log.println( "Cannot open directory '%s'", subDir.path() );
      log.unindent();
      log.println( "}" );

      throw Exception( "Library initialisation failure" );
    }

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "png" ) && !file->hasExtension( "jpeg" ) &&
          !file->hasExtension( "jpg" ) )
      {
        continue;
      }

      String name = subDir.name() + String( "/" ) + file->baseName();

      log.println( "%s", name.cstr() );

      if( textureIndices.contains( name ) ) {
        throw Exception( "Duplicated texture '" + name + "'" );
      }

      textureIndices.add( name, textures.length() );
      textures.add( Resource( name, file->path() ) );
    }
    subDirList.dealloc();
  }

  log.unindent();
  log.println( "}" );
}

void Library::initSounds()
{
  File dir;
  File subDir;
  DArray<File> dirList;
  DArray<File> subDirList;

  log.println( "sounds (*.wav in 'snd') {" );
  log.indent();

  dir.setPath( "snd" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'snd'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    subDir.setPath( file->path() );
    if( !subDir.ls( &subDirList ) ) {
      free();

      log.println( "Cannot open directory '%s'", subDir.path() );
      log.unindent();
      log.println( "}" );

      throw Exception( "Library initialisation failure" );
    }

    foreach( file, subDirList.citer() ) {
      if( !file->hasExtension( "wav" ) ) {
        continue;
      }

      String name = subDir.name() + String( "/" ) + file->baseName();

      log.println( "%s", name.cstr() );

      soundIndices.add( name, sounds.length() );
      sounds.add( Resource( name, file->path() ) );
    }
    subDirList.dealloc();
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
}

void Library::initCaela()
{
  File dir;
  DArray<File> dirList;

  log.println( "Caela (*.ozcCaelum in 'caelum') {" );
  log.indent();

  dir.setPath( "caelum" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'caelum'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozcCaelum" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    caelumIndices.add( name, caela.length() );
    caela.add( Resource( name, file->path() ) );
  }

  log.unindent();
  log.println( "}" );
}

void Library::initBuildCaela()
{
  File dir;
  DArray<File> dirList;

  log.println( "Caela (*.rc in 'caelum') {" );
  log.indent();

  dir.setPath( "caelum" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'caelum'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    caelumIndices.add( name, caela.length() );
    caela.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
}

void Library::initTerrae()
{
  File dir;
  DArray<File> dirList;

  log.println( "Terrae (*.ozTerra/*.ozcTerra in 'terra') {" );
  log.indent();

  dir.setPath( "terra" );
  if( !dir.ls( &dirList) ) {
    free();

    log.println( "Cannot open directory 'terra'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozTerra" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    terraIndices.add( name, terras.length() );
    terras.add( Resource( name, file->path() ) );
  }

  log.unindent();
  log.println( "}" );
}

void Library::initBuildTerrae()
{
  File dir;
  DArray<File> dirList;

  log.println( "Terrains (*.rc in 'terra') {" );
  log.indent();

  dir.setPath( "terra" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'terra'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    terraIndices.add( name, terras.length() );
    terras.add( Resource( name, file->path() ) );
  }

  log.unindent();
  log.println( "}" );
}

void Library::initBSPs()
{
  File dir;
  DArray<File> dirList;

  log.println( "BSP structures (*.ozBSP/*.ozcBSP in 'bsp') {" );
  log.indent();

  dir.setPath( "bsp" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'bsp'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( !file->hasExtension( "ozBSP" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    bspIndices.add( name, bsps.length() );
    bsps.add( Resource( name, file->path() ) );

    // read bounds
    if( !file->map() ) {
      throw Exception( "Cannot mmap BSP to read class info" );
    }

    InputStream is = file->inputStream();

    bspClasses.add();
    BSPClass& clazz = bspClasses.last();

    clazz.bounds.mins = is.readPoint3();
    clazz.bounds.maxs = is.readPoint3();

    int nSounds = is.readInt();
    for( int i = 0; i < nSounds; ++i ) {
      clazz.sounds.add( soundIndex( is.readString() ) );
    }

    clazz.title       = gettext( is.readString() );
    clazz.description = gettext( is.readString() );

    file->unmap();
  }

  log.unindent();
  log.println( "}" );
}

void Library::initBuildBSPs()
{
  File dir;
  DArray<File> dirList;

  log.println( "BSP structures (*.rc in 'data/maps') {" );
  log.indent();

  dir.setPath( "data/maps" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'data/maps'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    bspIndices.add( name, bsps.length() );
    bsps.add( Resource( name, file->path() ) );
  }

  log.unindent();
  log.println( "}" );
}

void Library::initModels()
{
  File dir;
  DArray<File> dirList;

  log.println( "models (*.ozcSMM, *.ozcMD2, *.ozcMD3 in 'mdl') {" );
  log.indent();

  dir.setPath( "mdl" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'mdl'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "ozcSMM" ) && !file->hasExtension( "ozcMD2" ) &&
        !file->hasExtension( "ozcMD3" ) )
    {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    if( modelIndices.contains( name ) ) {
      throw Exception( "Duplicated model '" + name + "'" );
    }

    modelIndices.add( name, models.length() );
    models.add( Resource( name, file->path() ) );
  }
  dirList.dealloc();

  log.unindent();
  log.println( "}" );
}

void Library::initBuildModels()
{
  File dir;
  DArray<File> dirList;

  log.println( "models (directories in 'mdl') {" );
  log.indent();

  dir.setPath( "mdl" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'mdl'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.iter() ) {
    if( file->getType() != File::DIRECTORY ) {
      continue;
    }

    String name = file->name();

    log.println( "%s", name.cstr() );

    modelIndices.add( name, models.length() );
    models.add( Resource( name, file->path() ) );
  }

  log.unindent();
  log.println( "}" );
}

void Library::initMusic()
{
  File dir;
  DArray<File> dirList;

  log.println( "music (*.oga, *.mp3 in 'music') {" );
  log.indent();

  dir.setPath( "music" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'music'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "oga" ) && !file->hasExtension( "mp3" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    musics.add( Resource( name, file->path() ) );
  }

  log.unindent();
  log.println( "}" );
}

void Library::initNameLists()
{
  File dir;
  DArray<File> dirList;

  log.println( "name lists (*.txt in 'name') {" );
  log.indent();

  dir.setPath( "name" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'name'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "txt" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    nameListIndices.add( name, nameLists.length() );
    nameLists.add( Resource( name, file->path() ) );
  }

  log.unindent();
  log.println( "}" );
}

void Library::initFragPools()
{
  File dir;
  DArray<File> dirList;

  log.println( "name lists (*.txt in 'name') {" );
  log.indent();

  dir.setPath( "name" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'name'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "txt" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    nameListIndices.add( name, nameLists.length() );
    nameLists.add( Resource( name, file->path() ) );
  }

  log.unindent();
  log.println( "}" );
}

void Library::initClasses()
{
  OZ_REGISTER_BASECLASS( Object );
  OZ_REGISTER_BASECLASS( Dynamic );
  OZ_REGISTER_BASECLASS( Weapon );
  OZ_REGISTER_BASECLASS( Bot );
  OZ_REGISTER_BASECLASS( Vehicle );

  File dir;
  DArray<File> dirList;
  Config classConfig;

  log.println( "object classes (*.rc in 'class') {" );
  log.indent();

  dir.setPath( "class" );
  if( !dir.ls( &dirList ) ) {
    free();

    log.println( "Cannot open directory 'class'" );
    log.unindent();
    log.println( "}" );
    throw Exception( "Library initialisation failure" );
  }
  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    log.println( "%s", name.cstr() );

    if( objClasses.contains( name ) ) {
      classConfig.clear();
      throw Exception( "Duplicated class '" + name + "'" );
    }
    if( !classConfig.load( file->path() ) ) {
      classConfig.clear( true );
      throw Exception( "Class parse error" );
    }

    const char* base = classConfig.get( "base", "" );
    if( String::isEmpty( base ) ) {
      classConfig.clear( true );
      throw Exception( "'base' missing in class description" );
    }

    const ObjectClass::CreateFunc* createFunc = baseClasses.find( base );
    if( createFunc == null ) {
      classConfig.clear( true );
      throw Exception( "Invalid class base '" + String( base ) + "'" );
    }

    // First we only add class instances, we don't initialise them as each class may have references
    // to other classes that have not been created yet.
    objClasses.add( name, ( *createFunc )() );

    // index device, imago and audio classes
    const char* sDevice = classConfig.get( "deviceType", "" );
    if( !String::isEmpty( sDevice ) ) {
      deviceIndices.include( sDevice, deviceIndices.length() );
    }

    const char* sImago = classConfig.get( "imagoType", "" );
    if( !String::isEmpty( sImago ) ) {
      imagoIndices.include( sImago, imagoIndices.length() );
    }

    const char* sAudio = classConfig.get( "audioType", "" );
    if( !String::isEmpty( sAudio ) ) {
      audioIndices.include( sAudio, audioIndices.length() );
    }

    classConfig.clear( true );
  }

  nDeviceClasses = deviceIndices.length();
  nImagoClasses  = imagoIndices.length();
  nAudioClasses  = audioIndices.length();

  // initialise all classes
  foreach( classIter, objClasses.iter() ) {
    String path = "class/" + classIter.key() + ".rc";

    if( !classConfig.load( path ) ) {
      classConfig.clear( true );
      throw Exception( "Class parse error" );
    }

    classConfig.add( "name", classIter.key() );
    classIter.value()->initClass( &classConfig );
    classConfig.clear();
  }

  foreach( classIter, objClasses.iter() ) {
    ObjectClass* objClazz = classIter.value();

    // check that all items are valid
    for( int i = 0; i < objClazz->defaultItems.length(); ++i ) {
      const ObjectClass* itemClazz = objClazz->defaultItems[i];

      if( ( itemClazz->flags & ( Object::DYNAMIC_BIT | Object::ITEM_BIT ) ) !=
          ( Object::DYNAMIC_BIT | Object::ITEM_BIT ) )
      {
        throw Exception( "Invalid item clazz '" + itemClazz->name + "' in '" +
                         itemClazz->name + "'" );
      }
    }

    // fill allowedUsers for weapons
    WeaponClass* weaponClazz = dynamic_cast<WeaponClass*>( objClazz );

    if( weaponClazz != null ) {
      int underscore = weaponClazz->name.index( '_' );
      if( underscore == -1 ) {
        throw Exception( "Weapon class file must be named <botClass>_weapon.<weapon>.rc" );
      }

      String matchClassBaseName = weaponClazz->name.substring( 0, underscore );

      foreach( clazz, objClasses.citer() ) {
        String botClassBaseName = clazz.value()->name;

        int dot = botClassBaseName.index( '.' );
        if( dot != -1 ) {
          botClassBaseName = botClassBaseName.substring( 0, dot );
        }

        if( matchClassBaseName.equals( botClassBaseName ) ) {
          weaponClazz->allowedUsers.add( clazz.value() );
        }
      }
    }
  }

  foreach( classIter, objClasses.citer() ) {
    // check if weaponItem is a valid weapon for bots
    if( classIter.value()->flags & Object::BOT_BIT ) {
      const BotClass* botClazz = static_cast<const BotClass*>( classIter.value() );

      if( botClazz->weaponItem != -1 ) {
        if( uint( botClazz->weaponItem ) >= uint( botClazz->defaultItems.length() ) ) {
          throw Exception( "Invalid weaponItem for '" + botClazz->name + "'" );
        }

        const ObjectClass* itemClazz = botClazz->defaultItems[botClazz->weaponItem];
        // we already checked it the previous loop it's non-null and a valid item
        const WeaponClass* weaponClazz = dynamic_cast<const WeaponClass*>( itemClazz );

        if( weaponClazz == null ) {
          throw Exception( "Default weapon of '" + botClazz->name + "' is of a non-weapon class" );
        }
        else if( !weaponClazz->allowedUsers.contains( botClazz ) ) {
          throw Exception( "Default weapon of '" + botClazz->name +
                           "' is not allowed for this bot class" );
        }
      }
    }
  }

  log.unindent();
  log.println( "}" );
}

void Library::init()
{
  bspClasses.alloc( 64 );
  shaders.alloc( 64 );
  textures.alloc( 256 );
  sounds.alloc( 256 );
  caela.alloc( 16 );
  terras.alloc( 16 );
  bsps.alloc( 64 );
  models.alloc( 256 );
  musics.alloc( 64 );
  nameLists.alloc( 16 );
  fragPools.alloc( 16 );

  log.println( "Library mapping resources {" );
  log.indent();

  initShaders();
  initTextures();
  initSounds();
  initCaela();
  initTerrae();
  initBSPs();
  initModels();
  initMusic();
  initNameLists();
  initFragPools();
  initClasses();

  log.unindent();
  log.println( "}" );
}

void Library::buildInit()
{
  bspClasses.alloc( 64 );
  shaders.alloc( 64 );
  textures.alloc( 256 );
  sounds.alloc( 256 );
  caela.alloc( 16 );
  terras.alloc( 16 );
  bsps.alloc( 64 );
  models.alloc( 256 );
  musics.alloc( 64 );
  nameLists.alloc( 16 );
  fragPools.alloc( 16 );

  log.println( "Library mapping resources {" );
  log.indent();

  initShaders();
  initBuildTextures();
  initSounds();
  initBuildCaela();
  initBuildTerrae();
  initBuildBSPs();
  initBuildModels();
  initMusic();
  initNameLists();
  initFragPools();
  initClasses();

  log.unindent();
  log.println( "}" );
}

void Library::free()
{
  textureIndices.clear();
  textureIndices.dealloc();
  soundIndices.clear();
  soundIndices.dealloc();
  shaderIndices.clear();
  shaderIndices.dealloc();
  terraIndices.clear();
  terraIndices.dealloc();
  caelumIndices.clear();
  caelumIndices.dealloc();
  bspIndices.clear();
  bspIndices.dealloc();
  modelIndices.clear();
  modelIndices.dealloc();
  nameListIndices.clear();
  nameListIndices.dealloc();

  textures.clear();
  textures.dealloc();
  sounds.clear();
  sounds.dealloc();
  shaders.clear();
  shaders.dealloc();
  terras.clear();
  terras.dealloc();
  caela.clear();
  caela.dealloc();
  bsps.clear();
  bsps.dealloc();
  models.clear();
  models.dealloc();
  nameLists.clear();
  nameLists.dealloc();
  musics.clear();
  musics.dealloc();

  bspClasses.clear();
  bspClasses.dealloc();
  baseClasses.clear();
  baseClasses.dealloc();
  objClasses.free();
  objClasses.dealloc();
}

}
}

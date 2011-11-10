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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/modules/MusicPlayerModule.cpp
 */

#include "stable.hpp"

#include "client/modules/MusicPlayerModule.hpp"

#include "client/modules/MusicPlayer.hpp"

#include "client/ui/UI.hpp"

namespace oz
{
namespace client
{

MusicPlayerModule musicPlayerModule;

MusicPlayerModule::MusicPlayerModule() : musicPlayer( null )
{}

void MusicPlayerModule::load()
{
  musicPlayer = new ui::MusicPlayer();
  ui::ui.root->add( musicPlayer );
  ui::ui.root->focus( ui::ui.loadingScreen );
}

void MusicPlayerModule::unload()
{
  if( musicPlayer != null ) {
    ui::ui.root->remove( musicPlayer );
    musicPlayer = null;
  }
}

}
}

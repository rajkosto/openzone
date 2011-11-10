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
 * @file client/MenuStage.cpp
 */

#include "stable.hpp"

#include "client/MenuStage.hpp"

#include "client/GameStage.hpp"
#include "client/Render.hpp"
#include "client/Sound.hpp"

#include "client/ui/UI.hpp"

namespace oz
{
namespace client
{

MenuStage menuStage;

bool MenuStage::update()
{
  ui::ui.update();

  return !doExit;
}

void MenuStage::present()
{
  render.draw( Render::DRAW_UI_BIT );
  render.sync();
}

void MenuStage::load()
{
  ui::mouse.load();
  ui::ui.root->add( new ui::MainMenu() );
  ui::mouse.doShow = true;
  ui::mouse.buttons = 0;
  ui::mouse.currButtons = 0;

  sound.update();

  ui::ui.showLoadingScreen( false );
}

void MenuStage::unload()
{
  ui::mouse.doShow = false;
  ui::mouse.unload();
}

void MenuStage::init()
{
  doExit = false;
}

void MenuStage::free()
{}

}
}

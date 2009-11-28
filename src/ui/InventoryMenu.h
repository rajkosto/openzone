/*
 *  InventoryMenu.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Frame.h"

namespace oz
{
namespace client
{
namespace ui
{

  class InventoryMenu : public Frame
  {
    private:

      static const int   ICON_SIZE   = 32;
      static const int   SLOT_SIZE   = 64;
      static const float SLOT_DIMF   = 32.0f;
      static const int   COLS        = 10;
      static const int   ROWS        = 4;
      static const int   HEADER_SIZE = 20;
      static const int   FOOTER_SIZE = 40;

      uint useTexId;
      int  taggedIndex;
      int  row;

    protected:

      virtual void onMouseEvent();
      virtual void onDraw();

    public:

      explicit InventoryMenu();
      ~InventoryMenu();

  };

}
}
}
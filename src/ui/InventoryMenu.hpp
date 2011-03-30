/*
 *  InventoryMenu.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "ui/Frame.hpp"

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
      static const float SLOT_DIMF;
      static const int   COLS        = 8;
      static const int   ROWS        = 2;
      static const int   HEADER_SIZE = 20;
      static const int   FOOTER_SIZE = 32;

      uint useTexId;
      int  tagged;
      int  row;

    protected:

      virtual bool onMouseEvent();
      virtual void onDraw();

    public:

      InventoryMenu();
      virtual ~InventoryMenu();

  };

}
}
}

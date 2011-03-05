/*
 *  LoadingArea.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "ui/Area.hpp"

namespace oz
{
namespace client
{
namespace ui
{

  class LoadingArea : public Area
  {
    friend class UI;

    private:

    protected:

      virtual void onDraw();

    public:

      explicit LoadingArea();
      virtual ~LoadingArea();

  };

}
}
}
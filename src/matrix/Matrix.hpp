/*
 *  Matrix.hpp
 *
 *  World model
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Synapse.hpp"
#include "matrix/Bot.hpp"

namespace oz
{

  class Matrix
  {
    private:

      void loadStressTest();
      void loadSample();

    public:

      static const float MAX_VELOCITY2;

      // thread synchronisation
      SDL_semaphore* semaphore;

      void init();
      void free();

      void load( InputStream* istream );
      void unload( OutputStream* ostream );

      void cleanObjects();
      void update();

  };

  extern Matrix matrix;

}

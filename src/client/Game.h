/*
 *  Client.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{
namespace client
{

  class Game
  {
    public:

      struct Input
      {
        ubyte *keys;
        ubyte oldKeys[SDLK_LAST];

        struct Mouse
        {
          int  x;
          int  y;
          byte b;
        }
        mouse;
      };

      enum State
      {
        GAME,
        GAME_INTERFACE,
        MENU
      };

    private:

      static const float FREECAM_SLOW_SPEED;
      static const float FREECAM_FAST_SPEED;

      float mouseXSens;
      float mouseYSens;

      float keyXSens;
      float keyYSens;

      float moveStep;
      float runStep;

      bool  fastMove;
      int   botRequestTicket;

    public:

      Input input;
      State state;

      bool init();
      void start();

      bool update( int time );

      void stop();
      void free();
  };

  extern Game game;

}
}

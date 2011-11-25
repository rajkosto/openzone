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
 * @file client/Loader.hpp
 */

#pragma once

#include "client/common.hpp"

namespace oz
{
namespace client
{

class Loader
{
  private:

    static const int TICK_PERIOD               = 180 * Timer::TICKS_PER_SEC;  // 2 min

    static const int IMAGO_CLEAR_INTERVAL      = 5   * Timer::TICKS_PER_SEC;  //   5 s (+ 2 s)
    static const int IMAGO_CLEAR_LAG           = 2   * Timer::TICKS_PER_SEC;

    static const int FRAG_CLEAR_INTERVAL       = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 20 s)
    static const int FRAG_CLEAR_LAG            = 20  * Timer::TICKS_PER_SEC;

    static const int IMAGOCLASS_CLEAR_INTERVAL = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 40 s)
    static const int IMAGOCLASS_CLEAR_LAG      = 40  * Timer::TICKS_PER_SEC;

    static const int BSP_CLEAR_INTERVAL        = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 60 s)
    static const int BSP_CLEAR_LAG             = 60  * Timer::TICKS_PER_SEC;

    static const int SOUND_CLEAR_INTERVAL      = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 100 s)
    static const int SOUND_CLEAR_LAG           = 100 * Timer::TICKS_PER_SEC;

    static const int SOURCE_CLEAR_INTERVAL     = 1   * Timer::TICKS_PER_SEC;  //   1 s (+ ~0.5 s)
    static const int SOURCE_CLEAR_LAG          = 1   * Timer::TICKS_PER_SEC / 2;

    static const int AUDIO_CLEAR_INTERVAL      = 5   * Timer::TICKS_PER_SEC;  //   5 s (+ 3 s)
    static const int AUDIO_CLEAR_LAG           = 3   * Timer::TICKS_PER_SEC;

    int tick;

    // clean unused imagines and handle screenshots
    void cleanupRender();
    // stop playing stopped continuous sounds, clean up unused audios
    void cleanupSound();

public:

    void cleanup();
    void update();

    void makeScreenshot();

    void init();
    void free();

};

extern Loader loader;

}
}

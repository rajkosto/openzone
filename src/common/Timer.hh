/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 */

/**
 * @file common/Timer.hh
 *
 * Timer class.
 */

#pragma once

#include <ozCore/ozCore.hh>

namespace oz
{

/**
 * Class that keeps track of the current game time, time passed from the last rendered frame etc.
 */
class Timer
{
public:

  /// Number of ticks (game updates) per second.
  static const uint TICKS_PER_SEC = 60;

  /// Length of one tick in microseconds.
  static const uint TICK_MICROS = (1000000 + TICKS_PER_SEC / 2) / TICKS_PER_SEC;

  /// Length of one tick in milliseconds.
  static const uint TICK_MILLIS = (1000 + TICKS_PER_SEC / 2) / TICKS_PER_SEC;

  /// Length of one tick in seconds.
  static constexpr float TICK_TIME = 1.0f / float(TICKS_PER_SEC);

  ulong64 runMicros   = 0;    ///< Run time (game time plus dropped time).

  ulong64 ticks       = 0;    ///< Ticks from the start of the game.
  ulong64 micros      = 0;    ///< Microseconds from the start of the game.
  float   time        = 0.0f; ///< %Time from the start of the game is seconds.

  ulong64 nFrames     = 0;    ///< Number of rendered frames from the start of the game.
  uint    frameTicks  = 0;    ///< Ticks from the last rendered frame.
  uint    frameMicros = 0;    ///< Microseconds of game time from the last rendered frame.
  float   frameTime   = 0.0f; ///< Game time from the last rendered frame.

  /**
   * Reset all timer counters to zero.
   */
  void reset();

  /**
   * Add one tick to the counters.
   */
  void tick();

  /**
   * Add one frame to the counters and reset frame counters.
   */
  void frame();

  /**
   * Drop time.
   *
   * On some occasions (e.g. when game freezes for a moment because of loading) time has to be
   * dropped, otherwise main loop will strive to catch up and compensate that time with shorter
   * ticks. That would results in a period after each "freeze" during which simulation will run
   * faster than in real time.
   */
  void drop(uint micros);

};

/**
 * Global Timer instance.
 */
extern Timer timer;

}

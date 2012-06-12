/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file client/StrategicProxy.hh
 */

#pragma once

#include "client/Proxy.hh"
#include "client/ui/StrategicArea.hh"

namespace oz
{
namespace client
{

class StrategicProxy : public Proxy
{
  private:

    static const float MIN_HEIGHT;
    static const float MAX_HEIGHT;
    static const float DEFAULT_HEIGHT;
    static const float DEFAULT_ANGLE;
    static const float FREE_LOW_SPEED;
    static const float FREE_HIGH_SPEED;
    static const float RTS_LOW_SPEED;
    static const float RTS_HIGH_SPEED;
    static const float ZOOM_FACTOR;

    ui::StrategicArea* strategicArea;

    Point desiredPos;
    float height;
    bool  isFree;
    bool  isFreeFast;
    bool  isRTSFast;

  public:

    StrategicProxy();

    void begin() override;
    void end() override;

    void prepare() override;
    void update() override;

    void reset() override;

    void read( InputStream* istream ) override;
    void write( BufferStream* ostream ) const override;

};

}
}

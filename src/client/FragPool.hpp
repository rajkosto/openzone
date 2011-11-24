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
 * @file client/FragPool.hpp
 */

#pragma once

#include "matrix/Frag.hpp"

#include "client/common.hpp"

namespace oz
{
namespace client
{

class SMM;

class FragPool
{
  public:

    static const int UPDATED_BIT = 0x01;
    static const int FADEOUT_BIT = matrix::FragPool::FADEOUT_BIT;

    static const float FRAG_RADIUS;

  private:

    const matrix::FragPool* pool;

    Vector<SMM*> models;

  public:

    int flags;

    explicit FragPool( const matrix::FragPool* pool );
    ~FragPool();

    void draw( const Frag* frag );

};

}
}
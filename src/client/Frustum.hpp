/*
 *  Frustum.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/common.hpp"
#include "client/Camera.hpp"

namespace oz
{
namespace client
{

  class Frustum
  {
    private:

      float fovX, fovY;
      float sx, cx, sy, cy;
      Vec4  nLeft0, nRight0, nDown0, nUp0;
      Vec4  nLeft, nRight, nDown, nUp, nFront;
      float dLeft, dRight, dDown, dUp, dFront;

      uint visibility( const Vec4& p );
      uint visibility( const Vec4& p, float radius );

    public:

      float maxDistance;
      float radius;

      void init( float fovY, float aspect, float maxDistance );
      void update( float maxDistance );

      bool isVisible( const Vec4& p, float radius = 0.0f )
      {
        return
            p * nLeft  > dLeft  - radius &&
            p * nRight > dRight - radius &&
            p * nUp    > dUp    - radius &&
            p * nDown  > dDown  - radius &&
            p * nFront < dFront + radius;
      }

      bool isVisible( const Sphere& s, float factor = 1.0f )
      {
        return isVisible( s.p, s.r * factor );
      }

      bool isVisible( const AABB& bb, float factor = 1.0f )
      {
        return isVisible( bb.p, bb.radius * factor );
      }

      bool isVisible( const Bounds& b )
      {
        Vec4 dim = b.maxs - b.mins;
        return isVisible( ( b.mins + b.maxs ) / 2.0f, !dim );
      }

      bool isVisible( float x, float y, float radius )
      {
        Vec4 min = Vec4( x, y, -World::DIM );
        Vec4 max = Vec4( x, y,  World::DIM );

        return
            ( min * nLeft  > dLeft  - radius || max * nLeft  > dLeft  - radius ) &&
            ( min * nRight > dRight - radius || max * nRight > dRight - radius ) &&
            ( min * nUp    > dUp    - radius || max * nUp    > dUp    - radius ) &&
            ( min * nDown  > dDown  - radius || max * nDown  > dDown  - radius ) &&
            ( min * nFront < dFront + radius || max * nFront < dFront + radius );
      }

      // get min and max index for cells per each axis, which should be included in pvs
      void getExtrems( Span& span, const Vec4& p )
      {
        span.minX = max( int( ( p.x - radius + World::DIM ) * Cell::INV_SIZE ), 0 );
        span.minY = max( int( ( p.y - radius + World::DIM ) * Cell::INV_SIZE ), 0 );
        span.maxX = min( int( ( p.x + radius + World::DIM ) * Cell::INV_SIZE ), World::MAX - 1 );
        span.maxY = min( int( ( p.y + radius + World::DIM ) * Cell::INV_SIZE ), World::MAX - 1 );
      }

  };

  extern Frustum frustum;

}
}

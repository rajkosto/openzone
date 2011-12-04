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
 * @file oz/Quat.hpp
 */

#pragma once

#include "Vec4.hpp"

namespace oz
{

/**
 * Quaternion.
 *
 * @ingroup oz
 */
class Quat : public Vec4
{
  public:

    /// Zero quaternion.
    static const Quat ZERO;

    /// Quaternion representing rotation identity.
    static const Quat ID;

    /**
     * Create an uninitialised instance.
     */
    Quat() = default;

    /**
     * Create a quaternion with the given components.
     */
    OZ_ALWAYS_INLINE
    explicit Quat( float x, float y, float z, float w ) : Vec4( x, y, z, w )
    {}

    /**
     * Create from an array of 4 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Quat( const float* q ) : Vec4( q )
    {}

    /**
     * Create quaternion from a four component vector.
     */
    OZ_ALWAYS_INLINE
    explicit Quat( const Vec4& v ) : Vec4( v )
    {}

    /**
     * Quaternion with absolute components.
     */
    OZ_ALWAYS_INLINE
    Quat abs() const
    {
      return Quat( Math::fabs( x ), Math::fabs( y ), Math::fabs( z ), Math::fabs( w ) );
    }

    /**
     * Conjugate quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat operator * () const
    {
      return Quat( -x, -y, -z, w );
    }

    /**
     * Unit quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat operator ~ () const
    {
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
      return Quat( x * k, y * k, z * k, w * k );
    }

    /**
     * Approximate unit quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat fastUnit() const
    {
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
      return Quat( x * k, y * k, z * k, w * k );
    }

    /**
     * Original quaternion.
     */
    OZ_ALWAYS_INLINE
    const Quat& operator + () const
    {
      return *this;
    }

    /**
     * Opposite quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat operator - () const
    {
      return Quat( -x, -y, -z, -w );
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    Quat operator + ( const Quat& q ) const
    {
      return Quat( x + q.x, y + q.y, z + q.z, w + q.w );
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    Quat operator - ( const Quat& q ) const
    {
      return Quat( x - q.x, y - q.y, z - q.z, w - q.w );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    Quat operator * ( float k ) const
    {
      return Quat( x * k, y * k, z * k, w * k );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    friend Quat operator * ( float k, const Quat& q )
    {
      return Quat( k * q.x, k * q.y, k * q.z, k * q.w );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    Quat operator ^ ( const Quat& q ) const
    {

      return Quat( w*q.x + x*q.w + y*q.z - z*q.y,
                   w*q.y + y*q.w + z*q.x - x*q.z,
                   w*q.z + z*q.w + x*q.y - y*q.x,
                   w*q.w - x*q.x - y*q.y - z*q.z );
    }

    /**
     * Quotient.
     */
    OZ_ALWAYS_INLINE
    Quat operator / ( float k ) const
    {
      hard_assert( k != 0.0f );

      k = 1.0f / k;
      return Quat( x * k, y * k, z * k, w * k );
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Quat& operator += ( const Quat& q )
    {
      x += q.x;
      y += q.y;
      z += q.z;
      w += q.w;
      return *this;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    Quat& operator -= ( const Quat& q )
    {
      x -= q.x;
      y -= q.y;
      z -= q.z;
      w -= q.w;
      return *this;
    }

    /**
     * Multiplication.
     */
    OZ_ALWAYS_INLINE
    Quat& operator *= ( float k )
    {
      x *= k;
      y *= k;
      z *= k;
      w *= k;
      return *this;
    }

    /**
     * Multiplication.
     */
    OZ_ALWAYS_INLINE
    Quat& operator ^= ( const Quat& q )
    {
      float tx = x, ty = y, tz = z;

      x = w*q.x + tx*q.w + ty*q.z - tz*q.y;
      y = w*q.y + ty*q.w + tz*q.x - tx*q.z;
      z = w*q.z + tz*q.w + tx*q.y - ty*q.x;
      w = w*q.w - tx*q.x - ty*q.y - tz*q.z;

      return *this;
    }

    /**
     * Division.
     */
    OZ_ALWAYS_INLINE
    Quat& operator /= ( float k )
    {
      hard_assert( k != 0.0f );

      k = 1.0f / k;
      x *= k;
      y *= k;
      z *= k;
      w *= k;
      return *this;
    }

    /**
     * Create quaternion for rotation around the given axis.
     */
    static Quat rotAxis( const Vec3& axis, float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( s * axis.x, s * axis.y, s * axis.z, c );
    }

    /**
     * Create quaternion for rotation around x axis.
     */
    static Quat rotX( float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( s, 0.0f, 0.0f, c );
    }

    /**
     * Create quaternion for rotation around y axis.
     */
    static Quat rotY( float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( 0.0f, s, 0.0f, c );
    }

    /**
     * Create quaternion for rotation around z axis.
     */
    static Quat rotZ( float theta )
    {
      float s, c;
      Math::sincos( theta * 0.5f, &s, &c );
      return Quat( 0.0f, 0.0f, s, c );
    }

    /**
     * rotZ ^ rotX ^ rotZ
     */
    static Quat rotZXZ( float heading, float pitch, float roll )
    {
      float hs, hc, ps, pc, rs, rc;

      Math::sincos( heading * 0.5f, &hs, &hc );
      Math::sincos( pitch   * 0.5f, &ps, &pc );
      Math::sincos( roll    * 0.5f, &rs, &rc );

      float hsps = hs * ps;
      float hcpc = hc * pc;
      float hspc = hs * pc;
      float hcps = hc * ps;

      return Quat( hcps * rc + hsps * rs,
                   hsps * rc - hcps * rs,
                   hspc * rc + hcpc * rs,
                   hcpc * rc - hspc * rs );
    }

    /**
     * Spherical linear interpolation between two rotations.
     */
    static Quat slerp( const Quat& a, const Quat& b, float t )
    {
      Quat  diff  = *a ^ b;
      float sine  = Math::sqrt( 1.0f - diff.w*diff.w );
      float angle = 2.0f * Math::acos( diff.w );

      hard_assert( sine != 0.0f );

      float k = 1.0f / sine;
      return rotAxis( Vec3( diff.x * k, diff.y * k, diff.z * k ), t * angle );
    }

    /**
     * Approximate spherical linear interpolation between two similar rotations.
     */
    static Quat fastSlerp( const Quat& a, const Quat& b, float t )
    {
      Quat  diff = *a ^ b;
      float k    = diff.w < 0.0f ? -t : t;

      diff.x *= k;
      diff.y *= k;
      diff.z *= k;
      diff.w = Math::fastSqrt( 1.0f - diff.x*diff.x - diff.y*diff.y - diff.z*diff.z );

      return a ^ diff;
    }

};

}

/*
 *  Vec3.hpp
 *
 *  3D vector
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Math.hpp"

namespace oz
{

  class Vec3 : public Simd
  {
    public:

      static const Vec3 ZERO;

      OZ_ALWAYS_INLINE
      Vec3()
      {}

      OZ_ALWAYS_INLINE
      explicit Vec3( float x_, float y_, float z_, float w_ ) : Simd( x_, y_, z_, w_ )
      {}

      OZ_ALWAYS_INLINE
      explicit Vec3( float x_, float y_, float z_ ) : Simd( x_, y_, z_, 0.0f )
      {}

      OZ_ALWAYS_INLINE
      explicit Vec3( const float* v ) : Simd( v[0], v[1], v[2], 0.0f )
      {}

      OZ_ALWAYS_INLINE
      bool operator == ( const Vec3& v ) const
      {
        return x == v.x && y == v.y && z == v.z;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Vec3& v ) const
      {
        return x != v.x || y != v.y || z != v.z;
      }

      OZ_ALWAYS_INLINE
      operator const float* () const
      {
        return f;
      }

      OZ_ALWAYS_INLINE
      operator float* ()
      {
        return f;
      }

      OZ_ALWAYS_INLINE
      const float& operator [] ( int i ) const
      {
        hard_assert( 0 <= i && i < 3 );

        return f[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 3 );

        return f[i];
      }

      OZ_ALWAYS_INLINE
      Vec3 abs() const
      {
        return Vec3( Math::abs( x ), Math::abs( y ), Math::abs( z ) );
      }

      OZ_ALWAYS_INLINE
      float operator ! () const
      {
        return Math::sqrt( x*x + y*y + z*z );
      }

      OZ_ALWAYS_INLINE
      float fastL() const
      {
         return Math::fastSqrt( x*x + y*y + z*z );
      }

      OZ_ALWAYS_INLINE
      float sqL() const
      {
        return x*x + y*y + z*z;
      }

      OZ_ALWAYS_INLINE
      Vec3 operator ~ () const
      {
        hard_assert( x*x + y*y + z*z > 0.0f );

        float k = 1.0f / Math::sqrt( x*x + y*y + z*z );
        return Vec3( x * k, y * k, z * k );
      }

      OZ_ALWAYS_INLINE
      Vec3 fastUnit() const
      {
        hard_assert( x*x + y*y + z*z > 0.0f );

        float k = Math::fastInvSqrt( x*x + y*y + z*z );
        return Vec3( x * k, y * k, z * k );
      }

      OZ_ALWAYS_INLINE
      bool isColinear( const Vec3& v, float epsilon ) const
      {
        float p1 = v.x * y * z;
        float p2 = v.y * x * z;
        float p3 = v.z * x * y;

        return Math::abs( p1 - p2 ) <= epsilon && Math::abs( p1 - p3 ) <= epsilon;
      }

      OZ_ALWAYS_INLINE
      Vec3 operator + () const
      {
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec3 operator - () const
      {
        return Vec3( -x, -y, -z );
      }

      OZ_ALWAYS_INLINE
      Vec3 operator + ( const Vec3& v ) const
      {
        return Vec3( x + v.x, y + v.y, z + v.z );
      }

      OZ_ALWAYS_INLINE
      Vec3 operator - ( const Vec3& v ) const
      {
        return Vec3( x - v.x, y - v.y, z - v.z );
      }

      OZ_ALWAYS_INLINE
      Vec3 operator * ( float k ) const
      {
        return Vec3( x * k, y * k, z * k );
      }

      OZ_ALWAYS_INLINE
      friend Vec3 operator * ( float k, const Vec3& v )
      {
        return Vec3( v.x * k, v.y * k, v.z * k );
      }

      OZ_ALWAYS_INLINE
      Vec3 operator / ( float k ) const
      {
        hard_assert( k != 0.0f );

        k = 1.0f / k;
        return Vec3( x * k, y * k, z * k );
      }

      OZ_ALWAYS_INLINE
      Vec3& operator += ( const Vec3& v )
      {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec3& operator -= ( const Vec3& v )
      {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec3& operator *= ( float k )
      {
        x *= k;
        y *= k;
        z *= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Vec3& operator /= ( float k )
      {
        hard_assert( k != 0.0f );

        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
        return *this;
      }

      // dot product
      OZ_ALWAYS_INLINE
      float operator * ( const Vec3& v ) const
      {
        return x*v.x + y*v.y + z*v.z;
      }

      // cross product
      OZ_ALWAYS_INLINE
      Vec3 operator ^ ( const Vec3& v ) const
      {
        return Vec3( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x );
      }

  };

}

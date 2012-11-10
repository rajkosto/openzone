/*
 * ozDynamics - OpenZone Dynamics Library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozDynamics/collision/Collider.cc
 */

#include "Collider.hh"

namespace oz
{

Collider::OverlapFunc* const Collider::dispatchMatrix[Shape::MAX][Shape::MAX] = {
  { boxBox,  boxCapsule,     boxMesh,     boxCompound      },
  { nullptr, capsuleCapsule, capsuleMesh, capsuleCompound  },
  { nullptr, nullptr,        meshMesh,    meshCompound     },
  { nullptr, nullptr,        nullptr,     compoundCompound }
};

bool Collider::boxBox( const Mat33& rot0, const Shape* box0_,
                       const Mat33& rot1, const Shape* box1_,
                       const Vec3& relPos, Result* result )
{
  const Box* box0 = static_cast<const Box*>( box0_ );
  const Box* box1 = static_cast<const Box*>( box1_ );

  Vec3  ext0      = box0->ext;
  Vec3  ext1      = box1->ext;
  Mat33 c         = ~rot0 * rot1;
  Mat33 d         = abs( c );

  Vec3  lastAxis  = Vec3::ZERO;
  float lastDepth = -1.0f;

  // Separating axes for faces of `box0`.
  for( int j = 0; j < 3; ++j ) {
    float r0    = ext0[j];
    float r1    = ext1 * d.row( j );
    float dist  = rot0[j] * relPos;
    float depth = r0 + r1 - dist;

    if( depth < 0.0f ) {
      return false;
    }
    else if( depth > lastDepth ) {
      lastAxis  = rot0[j];
      lastDepth = depth;
    }
  }

  // Separating axes for faces of `box1`.
  for( int j = 0; j < 3; ++j ) {
    float r0    = ext0 * d[j];
    float r1    = ext1[j];
    float dist  = rot1[j] * relPos;
    float depth = r0 + r1 - dist;

    if( depth < 0.0f ) {
      return false;
    }
    else if( depth > lastDepth ) {
      lastAxis  = rot1[j];
      lastDepth = depth;
    }
  }

  for( int i = 0; i < 3; ++i ) {
    int ia0 = ( 1 + i ) % 3;
    int ia1 = ( 2 + i ) % 3;

    for( int j = 0; j < 3; ++j ) {
      int ib0     = ( 1 + j ) % 3;
      int ib1     = ( 2 + j ) % 3;

      float ca0   = d[j][ia1];
      float ca1   = d[j][ia0];
      float cb0   = d[ib1][i];
      float cb1   = d[ib0][i];

      float r0    = ext0[ia0] * ca0 + ext0[ia1] * ca1;
      float r1    = ext1[ib0] * cb0 + ext1[ib1] * cb1;
      float dist  = abs( c[j][ia1] * ( rot0[ia0] * relPos ) - c[j][ia0] * ( rot0[ia1] * relPos ) );
      float depth = r0 + r1 - dist;

      if( depth < 0.0f ) {
        return false;
      }
      else if( depth > lastDepth ) {
        lastAxis  = rot0[i] ^ rot1[j];
        lastDepth = depth;
      }
    }
  }

  if( result != nullptr && lastDepth > result->depth ) {
    result->axis  = lastAxis;
    result->depth = lastDepth;
  }

  return true;
}

bool Collider::boxCapsule( const Mat33& rot0, const Shape* box_,
                           const Mat33& rot1, const Shape* capsule_,
                           const Vec3& relPos, Result* result )
{
  const Box*     box     = static_cast<const Box*>( box_ );
  const Capsule* capsule = static_cast<const Capsule*>( capsule_ );

  static_cast<void>( rot0 );
  static_cast<void>( rot1 );
  static_cast<void>( box );
  static_cast<void>( capsule );
  static_cast<void>( relPos );
  static_cast<void>( result );

  OZ_ERROR( "Not implemented" );
}

bool Collider::boxMesh( const Mat33& rot0, const Shape* box_,
                        const Mat33& rot1, const Shape* mesh_,
                        const Vec3& relPos, Result* result )
{
  const Box*  box  = static_cast<const Box*>( box_ );
  const Mesh* mesh = static_cast<const Mesh*>( mesh_ );

  static_cast<void>( rot0 );
  static_cast<void>( rot1 );
  static_cast<void>( box );
  static_cast<void>( mesh );
  static_cast<void>( relPos );
  static_cast<void>( result );

  OZ_ERROR( "Not implemented" );
}

bool Collider::boxCompound( const Mat33& rot0, const Shape* box_,
                            const Mat33& rot1, const Shape* compound_,
                            const Vec3& relPos, Result* result )
{
  const Box*      box      = static_cast<const Box*>( box_ );
  const Compound* compound = static_cast<const Compound*>( compound_ );

  bool overlaps = false;

  foreach( i, compound->citer() ) {
    Mat33 rot2 = rot1 * i->rot;

    OverlapFunc* func = dispatchMatrix[Shape::BOX][i->shape->type];
    overlaps |= func( rot0, box, rot2, i->shape, relPos + rot2 * i->off, result );
  }

  return overlaps;
}

bool Collider::capsuleCapsule( const Mat33& rot0, const Shape* capsule0_,
                               const Mat33& rot1, const Shape* capsule1_,
                               const Vec3& relPos, Result* result )
{
  const Capsule* capsule0 = static_cast<const Capsule*>( capsule0_ );
  const Capsule* capsule1 = static_cast<const Capsule*>( capsule1_ );

  float ext0    = capsule0->ext;
  float ext1    = capsule1->ext;
  float radius0 = capsule0->radius;
  float radius1 = capsule1->radius;

  float dot     = rot0.z * rot1.z;
  float denom   = 1.0f / ( 1.0f - dot*dot );
  float t0      = relPos * ( dot*rot1.z - rot0.z ) * denom;
  float t1      = relPos * ( dot*rot0.z - rot1.z ) * denom;

  t0 = clamp( t0, -ext0, +ext0 );
  t1 = clamp( t1, -ext1, +ext1 );

  Vec3  axis  = relPos + t1*rot1.z - t0*rot0.z;
  float dist2 = axis.sqN();
  float r     = radius0 + radius1;

  if( dist2 < r*r ) {
    if( result != nullptr ) {
      result->axis  = axis;
      result->depth = r - Math::fastSqrt( dist2 );
    }
    return true;
  }

  return false;
}

bool Collider::capsuleMesh( const Mat33& rot0, const Shape* capsule_,
                            const Mat33& rot1, const Shape* mesh_,
                            const Vec3& relPos, Result* result )
{
  const Capsule* capsule = static_cast<const Capsule*>( capsule_ );
  const Mesh*    mesh    = static_cast<const Mesh*>( mesh_ );

  static_cast<void>( rot0 );
  static_cast<void>( rot1 );
  static_cast<void>( capsule );
  static_cast<void>( mesh );
  static_cast<void>( relPos );
  static_cast<void>( result );

  OZ_ERROR( "Not implemented" );
}

bool Collider::capsuleCompound( const Mat33& rot0, const Shape* capsule_,
                                const Mat33& rot1, const Shape* compound_,
                                const Vec3& relPos, Result* result )
{
  const Capsule*  capsule  = static_cast<const Capsule*>( capsule_ );
  const Compound* compound = static_cast<const Compound*>( compound_ );

  bool overlaps = false;

  foreach( i, compound->citer() ) {
    Shape::Type type = i->shape->type;
    Mat33       rot2 = rot1 * i->rot;

    if( Shape::CAPSULE <= type ) {
      OverlapFunc* func = dispatchMatrix[Shape::CAPSULE][type];
      overlaps |= func( rot0, capsule, rot2, i->shape, relPos + rot2 * i->off, result );
    }
    else {
      OverlapFunc* func = dispatchMatrix[type][Shape::CAPSULE];
      overlaps |= func( rot2, i->shape, rot0, capsule, -relPos - rot2 * i->off, result );
    }
  }

  return overlaps;
}

bool Collider::meshMesh( const Mat33& rot0, const Shape* mesh0_,
                         const Mat33& rot1, const Shape* mesh1_,
                         const Vec3& relPos, Result* result )
{
  const Mesh* mesh0 = static_cast<const Mesh*>( mesh0_ );
  const Mesh* mesh1 = static_cast<const Mesh*>( mesh1_ );

  static_cast<void>( rot0 );
  static_cast<void>( rot1 );
  static_cast<void>( mesh0 );
  static_cast<void>( mesh1 );
  static_cast<void>( relPos );
  static_cast<void>( result );

  OZ_ERROR( "Not implemented" );
}

bool Collider::meshCompound( const Mat33& rot0, const Shape* mesh_,
                             const Mat33& rot1, const Shape* compound_,
                             const Vec3& relPos, Result* result )
{
  const Mesh*     mesh     = static_cast<const Mesh*>( mesh_ );
  const Compound* compound = static_cast<const Compound*>( compound_ );

  bool overlaps = false;

  foreach( i, compound->citer() ) {
    Shape::Type type = i->shape->type;
    Mat33       rot2 = rot1 * i->rot;

    if( Shape::MESH <= type ) {
      OverlapFunc* func = dispatchMatrix[Shape::MESH][type];
      overlaps |= func( rot0, mesh, rot2, i->shape, relPos + rot2 * i->off, result );
    }
    else {
      OverlapFunc* func = dispatchMatrix[type][Shape::MESH];
      overlaps |= func( rot2, i->shape, rot0, mesh, -relPos - rot2 * i->off, result );
    }
  }

  return overlaps;
}

bool Collider::compoundCompound( const Mat33& rot0, const Shape* compound0_,
                                 const Mat33& rot1, const Shape* compound1_,
                                 const Vec3& relPos, Result* result )
{
  const Compound* compound0 = static_cast<const Compound*>( compound0_ );
  const Compound* compound1 = static_cast<const Compound*>( compound1_ );

  Mat33 rot3[16];
  bool  overlaps = false;

  auto j = compound1->citer();
  for( int k = 0; j.isValid(); ++k, ++j ) {
    rot3[k] = rot1 * j->rot;
  }

  foreach( i, compound0->citer() ) {
    Shape::Type type0 = i->shape->type;
    Mat33       rot2  = rot0 * i->rot;

    auto j = compound1->citer();
    for( int k = 0; j.isValid(); ++k, ++j ) {
      Shape::Type type1 = i->shape->type;

      if( type0 <= type1 ) {
        OverlapFunc* func = dispatchMatrix[type0][type1];
        overlaps |= func( rot2, i->shape, rot3[k], j->shape,
                          relPos + rot3[k] * j->off - rot2 * i->off, result );
      }
      else {
        OverlapFunc* func = dispatchMatrix[type1][type0];
        overlaps |= func( rot3[k], j->shape, rot2, i->shape,
                          rot2 * i->off - rot3[k] * j->off - relPos, result );
      }
    }
  }

  return overlaps;
}

bool Collider::overlaps( const Body* body0, const Body* body1, Result* result )
{
  Shape* shape0 = body0->shape();
  Shape* shape1 = body1->shape();

  Shape::Type type0 = shape0->type;
  Shape::Type type1 = shape1->type;

  if( result != nullptr ) {
    result->depth = -1.0f;
  }

  if( type0 <= type1 ) {
    OverlapFunc* func = dispatchMatrix[type0][type1];
    return func( body0->rotMat, shape0, body1->rotMat, shape1, body1->pos - body0->pos, result );
  }
  else {
    OverlapFunc* func = dispatchMatrix[type1][type0];
    return func( body1->rotMat, shape1, body0->rotMat, shape0, body0->pos - body1->pos, result );
  }
}

}

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
 * @file ozDynamics/physics/Physics.cc
 */

#include "Physics.hh"

#include "DBody.hh"

#ifdef OZ_ODE

// If ODE is not compiled with single precision, this won't end good.
#define dSINGLE
#include <ode/ode.h>

namespace oz
{

static dWorldID      world;
static dJointGroupID contactGroup;

void Physics::add( DBody* body )
{
  dMass mass;

  switch( body->shape()->type ) {
    case Shape::BOX: {
      const Box* box = static_cast<const Box*>( body->shape() );

      dMassSetBox( &mass, 1.0f, box->ext.x, box->ext.y, box->ext.z );
      break;
    }
    case Shape::CAPSULE: {
      const Capsule* capsule = static_cast<const Capsule*>( body->shape() );

      dMassSetCapsule( &mass, 1.0f, 3, capsule->radius, 2.0f * capsule->ext );
      break;
    }
    case Shape::MESH: {
      dMassSetZero( &mass );
      break;
    }
    case Shape::COMPOUND: {
      dMassSetZero( &mass );
      break;
    }
  }

  body->odeId = dBodyCreate( world );

  dBodySetPosition( body->odeId, body->pos.x, body->pos.y, body->pos.z );
  dBodySetQuaternion( body->odeId, body->rot );
  dBodySetMass( body->odeId, &mass );
}

void Physics::erase( DBody* body )
{
  dBodyDestroy( body->odeId );
  body->odeId = nullptr;
}

void Physics::update()
{
  for( int i = 0; i < space->bodies.length(); ++i ) {
    const DBody* body0 = static_cast<const DBody*>( space->bodies[i] );

    for( int j = i + 1; j < space->bodies.length(); ++j ) {
      const DBody* body1 = static_cast<const DBody*>( space->bodies[j] );

      if( body0->odeId == nullptr && body1->odeId == nullptr ) {
        continue;
      }

      Collider::Result result;
      if( collider->overlaps( body0, body1, &result ) ) {
        Point p = Math::mix( body0->pos, body1->pos, 0.5f );

        dContact contact;
        contact.surface.mode       = dContactBounce | dContactSoftCFM;
        contact.surface.mu         = dInfinity;
        contact.surface.bounce     = 0.8f;
        contact.surface.bounce_vel = 0.1f;
        contact.surface.soft_cfm   = 0.001f;
        contact.geom.normal[0]     = result.axis[0];
        contact.geom.normal[1]     = result.axis[1];
        contact.geom.normal[2]     = result.axis[2];
        contact.geom.pos[0]        = p.x;
        contact.geom.pos[1]        = p.y;
        contact.geom.pos[2]        = p.z;
        contact.geom.depth         = result.depth;

        dJointID joint = dJointCreateContact( world, contactGroup, &contact );
        dJointAttach( joint, body0->odeId, body1->odeId );
      }
    }
  }

  dWorldQuickStep( world, 0.02f );

  foreach( i, space->bodies.citer() ) {
    DBody* body = static_cast<DBody*>( *i );

    body->pos    = Point( dBodyGetPosition( body->odeId ) );
    body->rot    = Quat( dBodyGetQuaternion( body->odeId ) );
    body->rotMat = Mat33( dBodyGetRotation( body->odeId ) );
  }
}

void Physics::init( Space* space_, Collider* collider_ )
{
  space    = space_;
  collider = collider_;

  dInitODE();

  world = dWorldCreate();
  dWorldSetGravity( world, 0.0f, 0.0f, -1.0f );

  contactGroup = dJointGroupCreate( 0 );
}

void Physics::destroy()
{
  dWorldDestroy( world );
  dJointGroupDestroy( contactGroup );

  dCloseODE();
}

}

#endif
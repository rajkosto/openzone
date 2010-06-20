/*
 *  Synapse.cpp
 *
 *  World manipulation interface.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.hpp"

#include "matrix/Synapse.hpp"

namespace oz
{

  Synapse synapse;

  Synapse::Synapse() : isSingle( true ), isServer( false ), isClient( false )
  {}

  void Synapse::genParts( int number, const Vec4& p,
                          const Vec4& velocity, float velocitySpread,
                          const Vec4& color, float colorSpread,
                          float restitution, float mass, float lifeTime )
  {
    float velocitySpread2 = velocitySpread / 2.0f;
    float colorSpread2 = colorSpread / 2.0f;

    for( int i = 0; i < number; ++i ) {
      Vec4 velDisturb = Vec4( velocitySpread * Math::frand() - velocitySpread2,
                              velocitySpread * Math::frand() - velocitySpread2,
                              velocitySpread * Math::frand() - velocitySpread2 );
      Vec4 colorDisturb = Vec4( colorSpread * Math::frand() - colorSpread2,
                                colorSpread * Math::frand() - colorSpread2,
                                colorSpread * Math::frand() - colorSpread2 );
      float timeDisturb = lifeTime * Math::frand();

      addPart( p, velocity + velDisturb, color + colorDisturb,
               restitution, mass, 0.5f * lifeTime + timeDisturb );
    }
  }

  void Synapse::update()
  {
    deleteObjects.free();

    actions.clear();

    addedStructs.clear();
    addedObjects.clear();
    addedParts.clear();

    removedStructs.clear();
    removedObjects.clear();
    removedParts.clear();
  }

  void Synapse::clearTickets()
  {
    putStructsIndices.clear();
    putObjectsIndices.clear();
    putPartsIndices.clear();
  }

}

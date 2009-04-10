/*
 *  VehicleClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "DynObjectClass.h"

namespace oz
{

  // TODO VehicleClass
  struct VehicleClass : DynObjectClass
  {
    static ObjectClass *init( const String &name, Config *config );
    virtual Object *create( const Vec3 &pos );
  };

}
/*
 *  OBJModel.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/OBJModel.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

  Pool<OBJModel, 256> OBJModel::pool;

  Model* OBJModel::create( const Object* obj )
  {
    OBJModel* model = new OBJModel();

    model->obj = obj;
    model->clazz = obj->clazz;
    model->objModel = context.loadOBJ( obj->clazz->modelName );
    return model;
  }

  OBJModel::~OBJModel()
  {
    context.releaseOBJ( clazz->modelName );
  }

  void OBJModel::draw( const Model* )
  {
    if( !objModel->isLoaded ) {
      return;
    }

    objModel->draw();
  }

}
}

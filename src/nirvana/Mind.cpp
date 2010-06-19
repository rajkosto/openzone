/*
 *  Mind.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.hpp"

#include "nirvana/Mind.hpp"

namespace oz
{
namespace nirvana
{

  Pool<Mind> Mind::pool;

  Mind* Mind::create( int botIndex )
  {
    Mind* mind = new Mind( botIndex );
    return mind;
  }

  Mind* Mind::read( InputStream* istream )
  {
    Mind* mind = new Mind( istream->readInt() );
    return mind;
  }

  Mind::~Mind()
  {}

  const char* Mind::type() const
  {
    return "";
  }

  void Mind::update()
  {}

  void Mind::write( OutputStream* ostream ) const
  {
    ostream->writeInt( botIndex );
  }

}
}

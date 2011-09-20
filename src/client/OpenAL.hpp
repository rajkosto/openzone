/*
 *  OpenAL.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include <AL/al.h>

namespace oz
{
namespace client
{

#ifdef NDEBUG
# define OZ_AL_CHECK_ERROR() void( 0 )
# else
# define OZ_AL_CHECK_ERROR() alCheckError( __FILE__, __LINE__, __PRETTY_FUNCTION__ )
#endif

  void alCheckError( const char* file, int line, const char* function );

}
}
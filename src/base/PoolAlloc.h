/*
 *  PoolAlloc.h
 *
 *  Pool allocator
 *  Base class for allocation of memory for same-size elements. All classes which inherit from
 *  PoolAlloc will have overloaded new and delete (BUT NOT new[] and delete[]) operators. Before
 *  you use it, you should create Pool<Type> for each class.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  template <class Type, int INDEX>
  class PoolAlloc
  {
    public:

      static Pool<Type, INDEX> pool;

#ifdef OZ_USE_POOLALLOC
      /**
       * Get an empty slot from pool.
       * @param
       * @return
       */
      void *operator new ( uint )
      {
        return pool.alloc();
      }

      /**
       * No placement new.
       * @param
       * @param
       * @return
       */
      void *operator new ( uint, void* )
      {
        assert( false );

        return null;
      }

      /**
       * Delete object
       * @param object
       */
      void operator delete ( void *ptr )
      {
        pool.free( (Type*) ptr );
      }
#endif

  };

  template <class Type, int INDEX>
  Pool<Type, INDEX> PoolAlloc<Type, INDEX>::pool;

}
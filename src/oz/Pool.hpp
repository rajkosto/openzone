/*
 *  Pool.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file Pool.hpp
 */

#include "common.hpp"

/**
 * @def OZ_STATIC_POOL_ALLOC( pool )
 *
 * Define overloaded <tt>new</code> and <tt>delete</tt> for a class that use the given pool.
 *
 * As <tt>new</tt>/<tt>delete</tt> are static functions so has to be the given pool.
 * The derived classes also need to have overloaded <tt>new</tt>/<tt>delete</tt> defined the
 * same way otherwise the ones from the base class will be used, which will not end good.
 *
 * If <tt>OZ_POOL_ALLOC</tt> is not defined, global <tt>new</tt> and <tt>delete</tt> operators
 * are used.
 */
#ifdef OZ_POOL_ALLOC

#define OZ_STATIC_POOL_ALLOC( pool ) \
  public:\
    void* operator new ( size_t ) { return pool.alloc(); } \
    void  operator delete ( void* ptr ) { if( ptr != null ) pool.dealloc( ptr ); }

#else

#define OZ_STATIC_POOL_ALLOC( pool )

#endif

/**
 * @def OZ_PLACEMENT_POOL_ALLOC( Type, SIZE )
 *
 * Define placement <tt>new</tt> for allocation from a pool and disable non-placement
 * <tt>new</tt> and <tt>delete</tt> for the class.
 *
 * The pool is given to new operator as an additional parameter. As delete cannot be provided,
 * object should be manually destructed and deallocated via <code>pool.dealloc( object )</code>.
 */
#define OZ_PLACEMENT_POOL_ALLOC( Type, SIZE ) \
  public: \
    void* operator new ( size_t, oz::Pool<Type, SIZE>& pool ) { return pool.alloc(); } \
    void  operator delete ( void* ptr, oz::Pool<Type, SIZE>& pool ) { pool.dealloc( ptr ); } \
  private: \
    void* operator new ( size_t ); \
    void  operator delete ( void* );

namespace oz
{

/**
 * Memory pool.
 *
 * Memory pool consists of a linked list of memory blocks, each an array of uninitialised elements
 * of the specified data type.
 */
template <class Type, int BLOCK_SIZE = 256>
class Pool
{
  static_assert( BLOCK_SIZE >= 2, "Pool block size must be at least 2" );

  private:

    /**
     * Slot that occupies memory for an object.
     */
    union Slot
    {
      char  content[ sizeof( Type ) ]; ///< Uninitialised memory for an object.
      Slot* nextSlot;                  ///< For an empty slot, a pointer to the next empty slot.
    };

    /**
     * Memory block.
     *
     * Block is an array that can hold up to <tt>BLOCK_SIZE</tt> elements. When we run out of
     * space we simply allocate another block. Once a block is allocated it cannot be freed any
     * more unless Pool is empty. That would be rarely possible due to fragmentation anyway.
     */
    struct Block
    {
      Slot   data[BLOCK_SIZE]; ///< Slots.
      Block* next;             ///< Pointer to the next block.

      /**
       * Create a new block.
       */
      explicit Block( Block* next_ ) : next( next_ )
      {
        for( int i = 0; i < BLOCK_SIZE - 1; ++i ) {
          data[i].nextSlot = &data[i + 1];
        }
        data[BLOCK_SIZE - 1].nextSlot = null;
      }
    };

    Block* firstBlock; ///< List of the allocated blocks.
    Slot*  freeSlot;   ///< List of free slots or null if none.
    int    size;       ///< Capacity.
    int    count;      ///< Number of occupied slots in the pool.

  public:

    /**
     * Create an empty pool, storage is allocated when the first allocation is made.
     */
    Pool() : firstBlock( null ), freeSlot( null ), size( 0 ), count( 0 )
    {}

    /**
     * Destructor.
     */
    ~Pool()
    {
      // there's a memory leak if count != 0
      hard_assert( count == 0 && size == 0 );
    }

  private:

    /**
     * No copying.
     */
    Pool( const Pool& );

    /**
     * No copying.
     */
    Pool& operator = ( const Pool& );

  public:

    /**
     * Allocate a new object.
     */
    void* alloc()
    {
#ifdef OZ_POOL_ALLOC
      ++count;

      if( freeSlot == null ) {
        firstBlock = new Block( firstBlock );
        freeSlot = &firstBlock->data[1];
        size += BLOCK_SIZE;
        return firstBlock->data[0].content;
      }
      else {
        Slot* slot = freeSlot;
        freeSlot = slot->nextSlot;
        return slot;
      }
#else
      return new char[ sizeof( Type ) ];
#endif
    }

    /**
     * Free the given object.
     */
    void dealloc( void* ptr )
    {
#ifdef OZ_POOL_ALLOC
      hard_assert( count != 0 );

      Slot* slot = reinterpret_cast<Slot*>( ptr );

# ifndef NDEBUG
      __builtin_memset( slot, 0xee, sizeof( Slot ) );
# endif

      slot->nextSlot = freeSlot;
      freeSlot = slot;
      --count;
#else
      delete[] reinterpret_cast<char*>( ptr );
#endif
    }

    /**
     * Number of used slots in the pool.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return count;
    }

    /**
     * True iff no slots are used.
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return count == 0;
    }

    /**
     * Number of allocated slots.
     */
    OZ_ALWAYS_INLINE
    int capacity() const
    {
      return size;
    }

    /**
     * Deallocate the storage.
     *
     * Frees all blocks allocated. It will not end good if something still uses memory allocated by
     * the pool.
     */
    void free()
    {
      hard_assert( count == 0 );

      Block* block = firstBlock;

      while( block != null ) {
        Block* next = block->next;
        delete block;

        block = next;
      }

      firstBlock = null;
      freeSlot   = null;
      size       = 0;
      count      = 0;
    }

};

}

/*
 *  Sparse.hpp
 *
 *  Sparse vector
 *  Similar to Vector, but it can have holes in the middle. When a new elements are added it first
 *  tries to occupy all free slots, new element is added to the end only if there is no holes in
 *  the middle.
 *  Type should provide int nextSlot[INDEX] field.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

namespace oz
{
  template <class Type, int INDEX = 0>
  class Sparse
  {
    public:

      /**
       * Constant Sparse iterator.
       */
      class CIterator : public oz::CIterator<Type>
      {
        private:

          typedef oz::CIterator<Type> B;

        public:

          /**
           * Default constructor returns a dummy passed iterator
           * @return
           */
          explicit CIterator() : B( null, null )
          {}

          /**
           * Make iterator for given Sparse. After creation it points to first element.
           * @param s
           */
          explicit CIterator( const Sparse& s ) : B( s.data, s.data + s.size )
          {}

          /**
           * Advance to next element.
           * @return
           */
          CIterator& operator ++ ()
          {
            do {
              ++B::elem;
            }
            while( B::elem != B::past && B::elem->nextSlot[INDEX] != -1 );

            return *this;
          }

        private:

          /**
           * No iterating backwards.
           * @return
           */
          CIterator& operator -- ();

      };

      /**
       * Sparse iterator.
       */
      class Iterator : public oz::Iterator<Type>
      {
        private:

          typedef oz::Iterator<Type> B;

        public:

          /**
           * Default constructor returns a dummy passed iterator
           * @return
           */
          explicit Iterator() : B( null, null )
          {}

          /**
           * Make iterator for given Sparse. After creation it points to first element.
           * @param s
           */
          explicit Iterator( const Sparse& s ) : B( s.data, s.data + s.size )
          {}

          /**
           * Advance to next element.
           * @return
           */
          Iterator& operator ++ ()
          {
            do {
              ++B::elem;
            }
            while( B::elem != B::past && B::elem->nextSlot[INDEX] != -1 );

            return *this;
          }

        private:

          /**
           * No iterating backwards.
           * @return
           */
          Iterator& operator -- ();

      };

    private:

      // Pointer to data array
      Type* data;
      // Size of data array
      int   size;
      // Number of used slots in the sparse sparse vector
      int   count;
      // List of free slots (by indices in data array, not by pointers), freeSlot == size if out of
      // free slots
      int   freeSlot;

      /**
       * Enlarge capacity by two times if there's not enough space to add another element.
       */
      void ensureCapacity()
      {
        assert( ( count == size ) == ( freeSlot == size ) );

        if( freeSlot == size ) {
          size *= 2;
          data = aRealloc( data, count, size );

          freeSlot = count;
          for( int i = count; i < size; ++i ) {
            data[i].nextSlot[INDEX] = i + 1;
          }
        }
      }

    public:

      /**
       * Create empty sparse vector with initial capacity 8.
       */
      explicit Sparse() : data( new Type[8] ), size( 8 ), count( 0 ), freeSlot( 0 )
      {
        for( int i = 0; i < size; ++i ) {
          data[i].nextSlot[INDEX] = i + 1;
        }
      }

      /**
       * Create empty sparse vector with given initial capacity.
       * @param initSize
       */
      explicit Sparse( int initSize ) : data( new Type[initSize] ), size( initSize ), count( 0 ),
          freeSlot( 0 )
      {
        for( int i = 0; i < size; ++i ) {
          data[i].nextSlot[INDEX] = i + 1;
        }
      }

      /**
       * Copy constructor.
       * @param s
       */
      Sparse( const Sparse& s ) : data( new Type[s.size] ), size( s.size ), count( s.count ),
          freeSlot( s.freeSlot )
      {
        aCopy( data, s.data, size );
      }

      /**
       * Move constructor.
       * @param s
       */
      Sparse( Sparse&& s ) : data( s.data ), size( s.size ), count( s.count ),
          freeSlot( s.freeSlot )
      {
        assert( s.size > 0 );

        s.data = null;
        s.size = 0;
        s.count = 0;
      }

      /**
       * Destructor.
       */
      ~Sparse()
      {
        delete[] data;
      }

      /**
       * Copy operator.
       * @param s
       * @return
       */
      Sparse& operator = ( const Sparse& s )
      {
        assert( &s != this );
        assert( s.size > 0 );

        // create new data array of the new data doesn't fit, keep the old one otherwise
        if( size < s.size || size == 0 ) {
          if( size != 0 ) {
            delete[] data;
          }
          data = new Type[s.size];
          size = s.size;
        }
        aCopy( data, s.data, s.size );
        count = s.count;
        freeSlot = s.freeSlot;
        return *this;
      }

      /**
       * Move operator.
       * @param s
       * @return
       */
      Sparse& operator = ( Sparse&& s )
      {
        assert( &s != this );
        assert( s.size > 0 );

        if( size != 0 ) {
          delete[] data;
        }
        data = s.data;
        size = s.size;
        count = s.count;
        freeSlot = s.freeSlot;

        s.data = null;
        s.size = 0;
        s.count = 0;
        return *this;
      }

      /**
       * Equality operator. Capacity of sparse vectors doesn't matter.
       * @param s
       * @return true if all elements in both sparse vectors are equal
       */
      bool operator == ( const Sparse& s ) const
      {
        if( count != s. count ) {
          return false;
        }
        for( int i = 0; i < size; ++i ) {
          if( data[i].nextSlot[INDEX] == -1 && data[i] != s.data[i] ) {
            return false;
          }
        }
        return true;
      }

      /**
       * Inequality operator. Capacity of sparse vectors doesn't matter.
       * @param s
       * @return false if all elements in both sparse vectors are equal
       */
      bool operator != ( const Sparse& s ) const
      {
        if( count != s. count ) {
          return true;
        }
        for( int i = 0; i < size; ++i ) {
          if( data[i].nextSlot[INDEX] == -1 && data[i] != s.data[i] ) {
            return true;
          }
        }
        return false;
      }

      /**
       * @return constant iterator for this vector
       */
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this vector
       */
      Iterator iter() const
      {
        return Iterator( *this );
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return constant pointer to data array
       */
      operator const Type* () const
      {
        return data;
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return non-constant pointer to data array
       */
      operator Type* ()
      {
        return data;
      }

      /**
       * @return number of elements (occupied slots) in the sparse vector
       */
      int length() const
      {
        return count;
      }

      /**
       * @return capacity of the sparse vector
       */
      int capacity() const
      {
        return size;
      }

      /**
       * @return true if sparse vector has no elements
       */
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * @param index
       * @return true if slot at given index in use
       */
      bool hasIndex( int index ) const
      {
        assert( 0 <= index && index < size );

        return data[index].nextSlot[INDEX] != -1;
      }

      /**
       * @param e
       * @return true if the element is found in the sparse vector
       */
      bool contains( const Type& e ) const
      {
        for( int i = 0; i < size; ++i ) {
          if( data[i].nextSlot[INDEX] == -1 && data[i] == e ) {
            return true;
          }
        }
        return false;
      }

      /**
       * @param i
       * @return constant reference i-th element
       */
      const Type& operator [] ( int i ) const
      {
        assert( 0 <= i && i < size );

        return data[i];
      }

      /**
       * @param i
       * @return reference i-th element
       */
      Type& operator [] ( int i )
      {
        assert( 0 <= i && i < size );

        return data[i];
      }

      /**
       * Find the first occurrence of an element.
       * @param e
       * @return index of first occurrence, -1 if not found
       */
      int index( const Type& e ) const
      {
        for( int i = 0; i < size; ++i ) {
          if( data[i].nextSlot[INDEX] == -1 && data[i] == e ) {
            return i;
          }
        }
        return -1;
      }

      /**
       * Find the last occurrence of an element.
       * @param e
       * @return index of last occurrence, -1 if not found
       */
      int lastIndex( const Type& e ) const
      {
        for( int i = size - 1; i >= 0; --i ) {
          if( data[i].nextSlot[INDEX] == -1 && data[i] == e ) {
            return i;
          }
        }
        return -1;
      }

      /**
       * Add an element to the end.
       * @param e
       */
      int operator << ( const Type& e )
      {
        ensureCapacity();

        int index = freeSlot;

        freeSlot = data[index].nextSlot[INDEX];
        data[index] = e;
        data[index].nextSlot[INDEX] = -1;
        ++count;

        return index;
      }

      /**
       * Create slot for a new element.
       * @return index at which the slot was created
       */
      int add()
      {
        ensureCapacity();

        int index = freeSlot;

        freeSlot = data[index].nextSlot[INDEX];
        data[index].nextSlot[INDEX] = -1;
        ++count;

        return index;
      }

      /**
       * Add an element.
       * @param e
       * @return index at which the element was inserted
       */
      int add( const Type& e )
      {
        ensureCapacity();

        int index = freeSlot;

        freeSlot = data[index].nextSlot[INDEX];
        data[index] = e;
        data[index].nextSlot[INDEX] = -1;
        ++count;

        return index;
      }

      /**
       * Remove the element at given position. A gap will remain there.
       * @param index
       */
      void remove( int index )
      {
        assert( 0 <= index && index < size );

        data[index].nextSlot[INDEX] = freeSlot;
        freeSlot = index;
        --count;
      }

      /**
       * Empty the sparse vector but don't delete the elements.
       */
      void clear()
      {
        count = 0;
        freeSlot = 0;

        for( int i = 0; i < size; ++i ) {
          data[i].nextSlot[INDEX] = i + 1;
        }
      }

  };

}
/*
 *  HashString.h
 *
 *  Chaining hashtable implementation with String key type
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{

  template <typename Type, int SIZE = 256>
  class HashString
  {
    private:

      struct Elem
      {
        String key;
        Type   value;
        Elem*  next[1];

        explicit Elem( const String& key_, const Type& value_, Elem* next_ ) :
            key( key_ ), value( value_ )
        {
          next[0] = next_;
        }

        explicit Elem( const String& key_, Elem* next_ ) : key( key_ )
        {
          next[0] = next_;
        }

        OZ_PLACEMENT_POOL_ALLOC( Elem, 0, SIZE )
      };

    public:

      /**
       * HashString iterator.
       */
      class Iterator : public IteratorBase<Elem>
      {
        private:

          typedef IteratorBase<Elem> B;

          Elem* const* const data;
          int index;

        public:

          /**
           * Default constructor returns a dummy passed iterator
           * @return
           */
          explicit Iterator() : B( null )
          {}

          /**
           * Make iterator for given HashString. After creation it points to first element.
           * @param t
           */
          explicit Iterator( const HashString& t ) : B( t.data[0] ), data( t.data ), index( 0 )
          {
            while( B::elem == null && index < SIZE - 1 ) {
              ++index;
              B::elem = data[index];
            }
          }

          /**
           * Advance to the next element.
           * @param
           */
          Iterator& operator ++ ()
          {
            assert( B::elem != null );

            if( B::elem->next[0] != null ) {
              B::elem = B::elem->next[0];
            }
            else if( index < SIZE - 1 ) {
              do {
                ++index;
                B::elem = data[index];
              }
              while( B::elem == null && index < SIZE - 1 );
            }
            else {
              B::elem = null;
            }
            return *this;
          }

          /**
           * @return current element's key
           */
          const String& key() const
          {
            return B::elem->key;
          }

          /**
           * @return reference to current element's value
           */
          Type& value()
          {
            return B::elem->value;
          }

          /**
           * @return constant reference to current element's value
           */
          const Type& value() const
          {
            return B::elem->value;
          }

          /**
           * @return pointer to current element
           */
          operator Type* ()
          {
            return &B::elem->value;
          }

          /**
           * @return constant pointer to current element
           */
          operator const Type* () const
          {
            return &B::elem->value;
          }

          /**
           * @return reference to current element
           */
          Type& operator * ()
          {
            return B::elem->value;
          }

          /**
           * @return constant reference to current element
           */
          const Type& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return non-constant access to member
           */
          Type* operator -> ()
          {
            return &B::elem->value;
          }

          /**
           * @return constant access to member
           */
          const Type* operator -> () const
          {
            return &B::elem->value;
          }

      };

      /**
       * HashString constant iterator.
       */
      class CIterator : public CIteratorBase<Elem>
      {
        private:

          typedef CIteratorBase<Elem> B;

          Elem* const* const data;
          int index;

        public:

          /**
           * Default constructor returns a dummy passed iterator
           * @return
           */
          explicit CIterator() : B( null )
          {}

          /**
           * Make iterator for given HashString. After creation it points to first element.
           * @param t
           */
          explicit CIterator( const HashString& t ) : B( t.data[0] ), data( t.data ), index( 0 )
          {
            while( B::elem == null && index < SIZE - 1 ) {
              ++index;
              B::elem = data[index];
            }
          }

          /**
           * Advance to the next element.
           * @param
           */
          CIterator& operator ++ ()
          {
            assert( B::elem != null );

            if( B::elem->next[0] != null ) {
              B::elem = B::elem->next[0];
            }
            else if( index < SIZE - 1 ) {
              do {
                ++index;
                B::elem = data[index];
              }
              while( B::elem == null && index < SIZE - 1 );
            }
            else {
              B::elem = null;
            }
            return *this;
          }

          /**
           * @return current element's key
           */
          const String& key() const
          {
            return B::elem->key;
          }

          /**
           * @return constant reference to current element's value
           */
          const Type& value() const
          {
            return B::elem->value;
          }

          /**
           * @return constant pointer to current element
           */
          operator const Type* () const
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element
           */
          const Type& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return constant access to member
           */
          const Type* operator -> () const
          {
            return &B::elem->value;
          }

      };

    private:

      Pool<Elem, 0, SIZE> pool;
      Elem*               data[SIZE];
      int                 count;

      /**
       * @param chainA
       * @param chainB
       * @return true if chains are equal length and all elements are equal
       */
      static bool areChainsEqual( const Elem* chainA, const Elem* chainB )
      {
        while( chainA != null && chainB != null ) {
          if( chainA->key != chainB->key || chainA->value != chainB->value ) {
            return false;
          }
          chainA = chainA->next[0];
          chainB = chainB->next[0];
        }
        // at least one is null, so (chainA == chainB) <=> (chainA == null && chainB == null)
        return chainA == chainB;
      }

      /**
       * Allocate space and make a copy of a given chain.
       * @param chain
       * @return pointer to first element of newly allocated chain
       */
      Elem* copyChain( const Elem* chain )
      {
        Elem* newChain = null;

        while( chain != null ) {
          newChain = new( pool ) Elem( chain->key, chain->value, newChain );
          chain = chain->next[0];
        }
        return newChain;
      }

      /**
       * Delete all elements in given chain.
       * @param chain
       */
      void freeChain( Elem* chain )
      {
        while( chain != null ) {
          Elem* next = chain->next[0];

          pool.free( chain );
          chain = next;
        }
      }

      /**
       * Delete all elements and their values in given chain.
       * @param chain
       */
      void freeChainAndValues( Elem* chain )
      {
        while( chain != null ) {
          Elem* next = chain->next[0];

          delete chain->value;
          pool.free( chain );
          chain = next;
        }
      }

    public:

      /**
       * Constructor.
       */
      explicit HashString() : count( 0 )
      {
        for( int i = 0; i < SIZE; ++i ) {
          data[i] = null;
        }
      }

      /**
       * Copy constructor.
       * @param t
       */
      HashString( const HashString& t ) : count( t.count )
      {
        for( int i = 0; i < SIZE; ++i ) {
          data[i] = copyChain( t.data[i] );
        }
      }

      /**
       * Default destructor.
       */
      ~HashString()
      {
        assert( count == 0 );

        pool.free();
      }

      /**
       * Copy operator.
       * @param t
       * @return
       */
      HashString& operator = ( const HashString& t )
      {
        assert( &t != this );
        assert( count == 0 );

        for( int i = 0; i < SIZE; ++i ) {
          data[i] = copyChain( t.data[i] );
        }
        count = t.count;

        return *this;
      }

      /**
       * Equality operator.
       * @param t
       * @return
       */
      bool operator == ( const HashString& t ) const
      {
        if( count != t.count ) {
          return false;
        }
        for( int i = 0; i < SIZE; ++i ) {
          if( !areChainsEqual( data[i], t.data[i] ) ) {
            return false;
          }
        }
        return true;
      }

      /**
       * Inequality operator.
       * @param t
       * @return
       */
      bool operator != ( const HashString& t ) const
      {
        if( count != t.count ) {
          return false;
        }
        for( int i = 0; i < SIZE; ++i ) {
          if( !areChainsEqual( data[i], t.data[i] ) ) {
            return true;
          }
        }
        return false;
      }

      /**
       * @return iterator for this HashString
       */
      Iterator iter() const
      {
        return Iterator( *this );
      }

      /**
       * @return constant iterator for this HashString
       */
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return number of elements
       */
      int length() const
      {
        return count;
      }

      /**
       * @return capacity
       */
      int capacity() const
      {
        return SIZE;
      }

      /**
       * @return load factor of hashtable (number of elements / capacity)
       */
      float loadFactor() const
      {
        return float( count ) / float( SIZE );
      }

      /**
       * @return true if HashString has no elements
       */
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * Find element with given value.
       * @param key
       * @return true if found
       */
      bool contains( const char* key ) const
      {
        int   i = String::hash( key ) % SIZE;
        Elem* p = data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            return true;
          }
          else {
            p = p->next[0];
          }
        }
        return false;
      }

      /**
       * Find element with given value
       * @param key
       * @return pointer to value of given key
       */
      Type* find( const char* key )
      {
        int   i = String::hash( key ) % SIZE;
        Elem* p = data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            return &p->value;
          }
          else {
            p = p->next[0];
          }
        }
        return null;
      }

      /**
       * Find element with given value
       * @param key
       * @return constant pointer to value of given key
       */
      const Type* find( const char* key ) const
      {
        int   i = String::hash( key ) % SIZE;
        Elem* p = data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            return &p->value;
          }
          else {
            p = p->next[0];
          }
        }
        return null;
      }

      /**
       * If given key exists, return reference to it's value.
       * Only use this function if you are certain that the key exists.
       * @param key
       * @return reference to value associated to the given key
       */
      Type& operator [] ( const char* key )
      {
        int   i = String::hash( key ) % SIZE;
        Elem* p = data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            return p->value;
          }
          else {
            p = p->next[0];
          }
        }

        assert( false );

        return data[0]->value;
      }

      /**
       * If given key exists, return constant reference to it's value.
       * Only use this function if you are certain that the key exists.
       * @param key
       * @return reference to value associated to the given key
       */
      const Type& operator [] ( const char* key ) const
      {
        int   i = String::hash( key ) % SIZE;
        Elem* p = data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            return p->value;
          }
          else {
            p = p->next[0];
          }
        }

        assert( false );

        return data[0]->value;
      }

      /**
       * Add new element. The key must not yet exist in this HashString.
       * @param key
       * @param value
       */
      Type* add( const char* key, const Type& value )
      {
        assert( !contains( key ) );

        int   i = String::hash( key ) % SIZE;
        Elem* elem = new( pool ) Elem( key, value, data[i] );

        data[i] = elem;
        ++count;

        assert( loadFactor() < 0.75f );

        return &data[i]->value;
      }

      /**
       * Add new element. The key must not yet exist in this HashString.
       * @param key
       * @param value
       */
      Type* add( const String& key, const Type& value )
      {
        assert( !contains( key ) );

        int   i = key.hash() % SIZE;
        Elem* elem = new( pool ) Elem( key, value, data[i] );

        data[i] = elem;
        ++count;

        assert( loadFactor() < 0.75f );

        return &data[i]->value;
      }

      /**
       * Remove element with given key.
       * @param key
       */
      void remove( const char* key )
      {
        int    i = String::hash( key ) % SIZE;
        Elem*  p = data[i];
        Elem** prev = &data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            *prev = p->next[0];
            pool.free( p );
            --count;
            return;
          }
          else {
            prev = &p->next[0];
            p = p->next[0];
          }
        }

        assert( false );
      }

      /**
       * Remove all elements.
       */
      void clear()
      {
        for( int i = 0; i < SIZE; ++i ) {
          freeChain( data[i] );
          data[i] = null;
        }
        count = 0;
      }

      /**
       * Remove all elements and delete their values. Only if values are pointers.
       */
      void free()
      {
        for( int i = 0; i < SIZE; ++i ) {
          freeChainAndValues( data[i] );
          data[i] = null;
        }
        count = 0;
      }

      /**
       * Deallocate memory from Pool.
       */
      void deallocate()
      {
        assert( count == 0 );

        pool.free();
      }

  };

}
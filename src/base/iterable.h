/*
 *  iterable.h
 *
 *  Basic iterator classes and utility templates.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  /**
   * Generalized iterator.
   * It should only be used as a base class. Following functions need to be implemented:<br>
   * <code>bool isPassed()</code><br>
   * <code>Iterator &operator ++ ()</code><br>
   * <code>Iterator &operator -- ()</code> (optional)<br>
   * and, of course, a sensible constructor.
   */
  template <class Type>
  class IteratorBase
  {
    protected:

      /**
       * Element which iterator is currently positioned at.
       */
      Type *elem;

      /**
       * @param start first element
       */
      explicit IteratorBase( Type *start ) : elem( start )
      {}

    public:

      /**
       * Returns true if iterators are at the same element. (That should do in most cases.)
       * @param e
       * @return
       */
      bool operator == ( const IteratorBase &i ) const
      {
        return elem == i.elem;
      }

      /**
       * Returns true if iterators are not at the same element. (That should do in most cases.)
       * @param e
       * @return
       */
      bool operator != ( const IteratorBase &i ) const
      {
        return elem != i.elem;
      }

      /**
       * @return pointer to current element
       */
      operator Type* ()
      {
        return elem;
      }

      /**
       * @return constant pointer to current element
       */
      operator const Type* () const
      {
        return elem;
      }

      /**
       * @return reference to current element
       */
      Type &operator * ()
      {
        return *elem;
      }

      /**
       * @return constant reference to current element
       */
      const Type &operator * () const
      {
        return *elem;
      }

      /**
       * @return non-constant access to member
       */
      Type *operator -> ()
      {
        return elem;
      }

      /**
       * @return constant access to member
       */
      const Type *operator -> () const
      {
        return elem;
      }

  };

  /**
   * Pointer iterator
   */
  template <class Type>
  class Iterator : public IteratorBase<Type>
  {
    private:

      // base class
      typedef IteratorBase<Type> B;

    protected:

      /**
       * Successor of the last element.
       * Is is used to determine when iterator becomes invalid.
       */
      const Type *past;

    public:

      /**
       * @param start first element for forward iterator or successor of last element for backward
       * iterator
       * @param past_ successor of last element for forward iterator or predecessor of first element
       * for backward iterator
       */
      explicit Iterator( Type *start, const Type *past_ ) : B( start ), past( past_ )
      {}

      /**
       * When iterator advances beyond last element, it becomes passed. It points to an invalid
       * location.
       * @return true if iterator is passed
       */
      bool isPassed() const
      {
        return B::elem == past;
      }

      /**
       * Advance to next element.
       */
      Iterator &operator ++ ()
      {
        assert( B::elem != past );

        B::elem++;
        return *this;
      }

      /**
       * Advance to previous element.
       */
      Iterator &operator -- ()
      {
        assert( B::elem != past );

        B::elem--;
        return *this;
      }

  };

  /**
   * \def foreach
   * Foreach macro can be used as in following example:
   * <pre>
   * Vector&lt;int&gt; v;
   * foreach( i, v.iterator() ) {
   *   printf( "%d ", *i );
   * }</pre>
   * To avoid some strange bugs, try not to use foreach as in following example:
   * <pre>
   * Vector&lt;int&gt; v;
   * foreach( i, Iterator( v ) ) {
   *   printf( "%d ", *i );
   * }</pre>
   * but rather transform it to
   * <pre>
   * Vector&lt;int&gt; v;
   * Vector&lt;int&gt;::Iterator iv( v );
   * foreach( i, iv ) {
   *   printf( "%d ", *i );
   * }</pre>
   * There's no need to add it to Katepart syntax highlighting as it is already there (Qt has some
   * similar foreach macro).
   */
# define foreach( i, startIterator ) \
  for( typeof( startIterator ) i( startIterator ); !i.isPassed(); ++i )

  /**
   * Compare all elements. (Like STL equal)
   * @param iA
   * @param iB
   * @return true if all elements are equal
   */
  template <class IteratorA, class IteratorB>
  inline bool iEquals( IteratorA &iDest, IteratorB &iSrc )
  {
    while( !iDest.isPassed() ) {
      if( *iDest != *iSrc ) {
        return false;
      }
      iDest++;
      iSrc++;
    }
    return true;
  }

  /**
   * Set all elements. (Like STL fill)
   * @param i
   * @param value
   */
  template <class Iterator, class Value>
  inline void iSet( Iterator &i, Value value )
  {
    while( !i.isPassed() ) {
      *i = value;
      i++;
    }
  }

  /**
   * Copy elements from first to last. (Like STL copy)
   * @param iA
   * @param iB
   */
  template <class IteratorA, class IteratorB>
  inline void iCopy( IteratorA &iDest, IteratorB &iSrc )
  {
    assert( &*iDest != &*iSrc );

    while( !iDest.isPassed() ) {
      *iDest = *iSrc;
      iDest++;
      iSrc++;
    }
  }

  /**
   * Copy elements from last to first.
   * @param iA
   * @param iB
   */
  template <class BackwardIteratorA, class BackwardIteratorB>
  inline void iReverseCopy( BackwardIteratorA &iDest, BackwardIteratorB &iSrc )
  {
    assert( &*iDest != &*iSrc );

    while( !iDest.isPassed() ) {
      iDest--;
      iSrc--;
      *iDest = *iSrc;
    }
  }

  /**
   * Find first occurence of given element. (Like STL find)
   * @param i
   * @param value
   * @return iterator at the elements found, passed iterator if not found
   */
  template <class Iterator, class Value>
  inline Iterator iIndex( Iterator &i, Value value )
  {
    while( !i.isPassed() ) {
      if( *i == value ) {
        break;
      }
      i++;
    }
    return i;
  }

  /**
   * Find last occurence of given element.
   * @param i
   * @param value
   * @return iterator at the elements found, passed iterator if not found
   */
  template <class BackwardIterator, class Value>
  inline BackwardIterator iLastIndex( BackwardIterator &i, Value value )
  {
    while( !i.isPassed() ) {
      i--;
      if( *i == value ) {
        break;
      }
    }
    return i;
  }

  /**
   * Call delete on all elements (that have been previously allocated with the new call).
   * @param i
   */
  template <class Iterator, class Type>
  inline void iFree( Iterator &i )
  {
    while( !i.isPassed() ) {
      Type *p = *i;
      i++;
      delete p;
    }
  }

}
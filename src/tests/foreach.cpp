/*
 *  foreach.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <SDL/SDL.h>
#include <SDL/SDL_main.h>

using namespace oz;

bool Alloc::isLocked = true;

struct Elem
{
  int value;

  Elem* prev[1];
  Elem* next[1];

  explicit Elem( int value_ ) : value( value_ ) {}
};

struct SparseElem
{
  int value;

  SparseElem()
  {}

  explicit SparseElem( int value_ ) : value( value_ )
  {}
};

int main( int, char** )
{
  Alloc::isLocked = false;

  List<Elem, 0> l;
  DList<Elem, 0> dl;
  Array<int, 5> a;
  Vector<int> v;
  SVector<int, 5> sv;
  Sparse<SparseElem> s;
  Map<int> m;
  HashIndex<int, 7> hi;
  HashString<int, 7> hs;

  List<Elem, 0> l1;
  DList<Elem, 0> dl1;
  Array<int, 5> a1;
  Vector<int> v1;
  SVector<int, 5> sv1;
  Sparse<SparseElem> s1;
  Map<int> m1;
  HashIndex<int, 7> hi1;
  HashString<int, 7> hs1;

  // 1
  l.add( new Elem( 1 ) );
  dl.add( new Elem( 1 ) );
  a[0] = 1;
  v.add( 1 );
  sv.add( 1 );
  s.add( SparseElem( 1 ) );
  m.include( 1 );
  hi.add( 101, 1 );
  hs.add( "101", 1 );

  // 2
  l.add( new Elem( 2 ) );
  dl.add( new Elem( 2 ) );
  a[1] = 2;
  v.add( 2 );
  sv.add( 2 );
  s.add( SparseElem( 2 ) );
  m.include( 2 );
  hi.add( 102, 2 );
  hs.add( "102", 2 );

  // 4
  l.add( new Elem( 4 ) );
  dl.add( new Elem( 4 ) );
  a[2] = 4;
  v.add( 4 );
  sv.add( 4 );
  s.add( SparseElem( 4 ) );
  m.include( 4 );
  hi.add( 104, 4 );
  hs.add( "104", 4 );

  // 3
  l.add( new Elem( 3 ) );
  dl.add( new Elem( 3 ) );
  a[3] = 3;
  v.add( 3 );
  sv.add( 3 );
  s.add( SparseElem( 3 ) );
  m.include( 3 );
  hi.add( 103, 3 );
  hs.add( "103", 3 );

  // 5
  l.add( new Elem( 5 ) );
  dl.add( new Elem( 5 ) );
  a[4] = 5;
  v.add( 5 );
  sv.add( 5 );
  s.add( SparseElem( 5 ) );
  m.include( 5 );
  hi.add( 105, 5 );
  hs.add( "105", 5 );

  swap( l, l1 );
  swap( dl, dl1 );
  swap( a, a1 );
  swap( v, v1 );
  swap( sv, sv1 );
  swap( s, s1 );
  swap( m, m1 );
  swap( hi, hi1 );
  swap( hs, hs1 );

  swap( l, l1 );
  swap( dl, dl1 );
  swap( a, a1 );
  swap( v, v1 );
  swap( sv, sv1 );
  swap( s, s1 );
  swap( m, m1 );
  swap( hi, hi1 );
  swap( hs, hs1 );

  v1.dealloc();
  s1.dealloc();
  m1.dealloc();
  hi1.dealloc();
  hs1.dealloc();

  foreach( i, l.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );

  foreach( i, dl.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );

  foreach( i, a.citer() ) {
    printf( "%d ", *i );
  }
  printf( "\n" );

  foreach( i, v.citer() ) {
    printf( "%d ", *i );
  }
  printf( "\n" );

  foreach( i, sv.citer() ) {
    printf( "%d ", *i );
  }
  printf( "\n" );

  foreach( i, s.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );

  foreach( i, m.citer() ) {
    printf( "%d ", i.key() );
  }
  printf( "\n" );

  foreach( i, hi.citer() ) {
    printf( "%d ", *i );
  }
  printf( "\n" );

  foreach( i, hs.citer() ) {
    printf( "%d ", *i );
  }
  printf( "\n" );

  l.free();
  dl.free();
  v.clear();
  v.dealloc();
  s.clear();
  s.dealloc();
  m.clear();
  m.dealloc();
  hi.clear();
  hi.dealloc();
  hs.clear();
  hs.dealloc();

  Alloc::isLocked = true;
  Alloc::printLeaks();
  return 0;
}

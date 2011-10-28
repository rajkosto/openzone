/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <SDL/SDL_main.h>

bool oz::Alloc::isLocked = true;

using namespace std;
using namespace oz;

template <typename Type, int STACK_SIZE>
inline void aSort( Type* array, int count )
{
  Type*  stack[STACK_SIZE];
  Type** sp = stack;
  Type*  first = array;
  Type*  last = array + count - 1;

  *( ++sp ) = first;
  *( ++sp ) = last;

  do {
    last = *( --sp );
    first = *( --sp );

    if( first < last ) {
      if( last - first > 10 ) {
        int pivotValue = *last;
        Type* top = first;
        Type* bottom = last - 1;

        do {
          while( top <= bottom && *top <= pivotValue ) {
            ++top;
          }
          while( top < bottom && *bottom > pivotValue ) {
            --bottom;
          }
          if( top >= bottom ) {
            break;
          }
          swap( *top, *bottom );
        }
        while( true );

        swap( *top, *last );

        *( ++sp ) = first;
        *( ++sp ) = top - 1;
        *( ++sp ) = top + 1;
        *( ++sp ) = last;
      }
      else {
        // selection sort
        for( Type* i = first; i < last; ) {
          Type* pivot = i;
          Type* min = i;
          ++i;

          for( Type* j = i; j <= last; ++j ) {
            if( *j < *min ) {
              min = j;
            }
          }
          swap( *pivot, *min );
        }
      }
    }
  }
  while( sp != stack );
}

template <typename Type>
inline void arSort( Type* first, Type* last )
{
  if( first < last ) {
    if( last - first > 1 ) {
      int pivotValue = *last;
      Type* top = first;
      Type* bottom = last - 1;

      do {
        while( top <= bottom && *top <= pivotValue ) {
          ++top;
        }
        while( top < bottom && *bottom > pivotValue ) {
          --bottom;
        }
        if( top >= bottom ) {
          break;
        }
        swap( *top, *bottom );
      }
      while( true );

      swap( *top, *last );

      arSort( first, top - 1 );
      arSort( top + 1, last );
    }
    else if( *first > *last ) {
      swap( *first, *last );
    }
  }
}

template <typename Type>
static void oaSort( Type* array, int begin, int end )
{
  int first = begin;
  int last = end - 1;

  if( first < last ) {
    if( first + 1 == last ) {
      if( array[first] > array[last] ) {
        swap( array[first], array[last] );
      }
    }
    else {
      int pivotValue = array[last];
      int top = first;
      int bottom = last - 1;

      do {
        while( top <= bottom && array[top] <= pivotValue ) {
          ++top;
        }
        while( top < bottom && array[bottom] > pivotValue ) {
          --bottom;
        }
        if( top < bottom ) {
          swap( array[top], array[bottom] );
        }
        else {
          break;
        }
      }
      while( true );

      swap( array[top], array[last] );
      oaSort( array, begin, top );
      oaSort( array, top + 1, end );
    }
  }
}

#define MAX 10000
#define TESTS 2000

int main( int, char** )
{
  oz::Alloc::isLocked = false;

  oz::System::catchSignals();

  SDL_Init( 0 );

  int array[MAX];

  srand( 32 );

  oz::uint t0 = SDL_GetTicks();
  for( int i = 0; i < TESTS; ++i ) {
    for( int j = 0; j < MAX; ++j ) {
      array[j] = rand() % MAX;
    }
    //sort( array, 0, MAX );
    //quickSort( array, MAX );
    //arSort( array, array + MAX - 1 );
    oz::aSort( array, MAX );
    //aSort<int, 100>( array, MAX );
  }
  printf( "%d ms\n", SDL_GetTicks() - t0 );
//   for( int i = 0; i < MAX; ++i ) {
//     printf( "%d ", array[i] );
//   }

  SDL_Quit();

  oz::Alloc::isLocked = true;
  oz::Alloc::printLeaks();
  return 0;
}

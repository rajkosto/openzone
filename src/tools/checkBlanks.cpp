/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file tools/checkBlanks.cpp
 */

#include <cstdio>
#include <cstdlib>

int main( int argc, char** argv )
{
  for( int i = 1; i < argc; ++i ) {
    FILE* file = fopen( argv[i], "rb" );

    if( file == nullptr ) {
      continue;
    }

    int nBlanks = 0;
    char ch;
    do {
      ch = char( fgetc( file ) );

      if( ch == '\n' || ch == EOF ) {
        ++nBlanks;
      }
      else {
        nBlanks = 0;
      }

      if( nBlanks == 3 ) {
        printf( "%s\n", argv[i] );
        break;
      }
    }
    while( ch != EOF );

    fclose( file );
  }
  return EXIT_SUCCESS;
}

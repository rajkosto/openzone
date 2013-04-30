/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozEngine/Builder.cc
 */

#include "Builder.hh"

#include <FreeImagePlus.h>
#ifdef OZ_NONFREE
# include <squish.h>
#endif

namespace oz
{

static const int DDSD_CAPS        = 0x00000001;
static const int DDSD_HEIGHT      = 0x00000002;
static const int DDSD_WIDTH       = 0x00000004;
static const int DDSD_PITCH       = 0x00000008;
static const int DDSD_PIXELFORMAT = 0x00001000;
static const int DDSD_MIPMAPCOUNT = 0x00020000;
static const int DDSD_LINEARSIZE  = 0x00080000;

static const int DDSDCAPS_COMPLEX = 0x00000008;
static const int DDSDCAPS_TEXTURE = 0x00001000;
static const int DDSDCAPS_MIPMAP  = 0x00400000;

static const int DDPF_ALPHAPIXELS = 0x00000001;
static const int DDPF_FOURCC      = 0x00000004;
static const int DDPF_RGB         = 0x00000040;
static const int DDPF_LUMINANCE   = 0x00020000;

bool Builder::buildDDS( const File& file, int options, OutputStream* ostream )
{
#ifndef OZ_NONFREE
  if( options & COMPRESSION_BIT ) {
    return false;
  }
#endif

  Buffer      buffer;
  InputStream istream;

  if( file.isMapped() ) {
    istream = file.inputStream();
  }
  else {
    buffer  = file.read();
    istream = buffer.inputStream();
  }

  fipImage    image;
  fipMemoryIO memoryIO( reinterpret_cast<ubyte*>( const_cast<char*>( istream.begin() ) ),
                        uint( istream.available() ) );

  if( !image.loadFromMemory( memoryIO ) ) {
    return false;
  }
  image.flipVertical();

  int width  = int( image.getWidth() );
  int height = int( image.getHeight() );
  int bpp    = int( image.getBitsPerPixel() );
  int pitch  = int( image.getScanWidth() );

  if( ( options & COMPRESSION_BIT ) && ( !Math::isPow2( width ) || !Math::isPow2( height ) ) ) {
    return false;
  }

  int nMipmaps = options & MIPMAPS_BIT ? Math::index1( max( width, height ) ) + 1 : 1;

  int flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  flags |= options & MIPMAPS_BIT ? DDSD_MIPMAPCOUNT : 0;
  flags |= options & COMPRESSION_BIT ? DDSD_LINEARSIZE : DDSD_PITCH;

  int caps = DDSDCAPS_TEXTURE;
  caps |= options & MIPMAPS_BIT ? DDSDCAPS_COMPLEX | DDSDCAPS_MIPMAP : 0;

  int pixelFlags = 0;
  pixelFlags |= bpp == 32 ? DDPF_ALPHAPIXELS : 0;
  pixelFlags |= options & COMPRESSION_BIT ? DDPF_FOURCC :
                bpp == 8 ? DDPF_LUMINANCE : DDPF_RGB;

  // Header beginning.
  ostream->writeChars( "DDS ", 4 );
  ostream->writeInt( 124 );
  ostream->writeInt( flags );
  ostream->writeInt( height );
  ostream->writeInt( width );
  ostream->writeInt( pitch ); // Will be overwritten later if compressed.
  ostream->writeInt( 0 );
  ostream->writeInt( nMipmaps );

  // Reserved int[11].
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );

  // Pixel format.
  ostream->writeInt( 32 );
  ostream->writeInt( pixelFlags );
  ostream->writeChars( bpp == 32 ? "DXT5" : "DXT1", 4 );
  ostream->writeInt( bpp );
  ostream->writeUInt( 0x00ff0000 );
  ostream->writeUInt( 0x0000ff00 );
  ostream->writeUInt( 0x000000ff );
  ostream->writeUInt( 0xff000000 );

  ostream->writeInt( caps );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );
  ostream->writeInt( 0 );

  for( int i = 0; i < nMipmaps; ++i ) {
    fipImage level = image;

    if( i != 0 ) {
      level.rescale( uint( width ), uint( height ),
                     options & QUALITY_BIT ? FILTER_CATMULLROM : FILTER_BOX );
      pitch = int( level.getScanWidth() );
    }

#ifdef OZ_NONFREE
    int squishFlags = bpp == 32 ? squish::kDxt5 : squish::kDxt5;
    squishFlags |= options & QUALITY_BIT ?
                   squish::kColourIterativeClusterFit | squish::kWeightColourByAlpha :
                   squish::kColourRangeFit;
#endif

    if( options & COMPRESSION_BIT ) {
#ifdef OZ_NONFREE
      level.convertTo32Bits();

      int   size   = squish::GetStorageRequirements( width, height, squishFlags );
      char* pixels = reinterpret_cast<char*>( level.accessPixels() );

      // Swap red and blue channels.
      for( int y = 0; y < height; ++y ) {
        for( int x = 0; x < width; ++x ) {
          swap( pixels[x * 4 + 0], pixels[x * 4 + 2] );
        }
        pixels += pitch;
      }

      squish::CompressImage( level.accessPixels(), width, height, ostream->forward( size ),
                             squishFlags );

      // Replace pitch with "linear size".
      if( i == 0 ) {
        int pos = ostream->tell();

        ostream->seek( 20 );
        ostream->writeInt( size );
        ostream->seek( pos );
      }
#endif
    }
    else {
      const char* pixels = reinterpret_cast<const char*>( level.accessPixels() );

      for( int i = 0; i < height; ++i ) {
        ostream->writeChars( pixels, width * ( bpp / 8 ) );
        pixels += pitch;
      }
    }

    width  = max( width / 2, 1 );
    height = max( height / 2, 1 );
  }
  return true;
}

}

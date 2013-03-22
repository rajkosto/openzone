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
 * @file ozEngine/GLTexture.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * OpenGL texture wrapper.
 */
class GLTexture
{
  public:

    /**
     * Build option bit enabling generation of mipmaps.
     */
    static const int MIPMAPS_BIT = 0x01;

    /**
     * Build option bit enabling texture compression.
     */
    static const int COMPRESSION_BIT = 0x02;

    /**
     * Build option bit forcing the highest quality compression and mipmap scaling.
     */
    static const int QUALITY_BIT = 0x04;

  private:

    uint textureId;      ///< OpenGL texture id.
    uint textureFormat;  ///< OpenGL internal texture format enumerator.
    int  textureMipmaps; ///< True iff mipmaps have been are loaded from file.

  public:

    /**
     * Convert given image to DDS format and optionally compress it and create mipmaps.
     *
     * Mipmap generation, S3 texture compression, and quality of compression and mipmap images
     * scaling can be controlled via `options` parameter.
     * @li `MIPMAPS_BIT` enables generation of mipmaps.
     * @li `COMPRESSION_BIT` enables S3 texture compression; DXT1 is used for images without an
     *     alpha channel and DXT5 for images with an alpha channel.
     *     Texture compression is enabled only if OZ_NONFREE is enabled on ozEngine build.
     * @li `QUALITY_BIT` enables highest quality but slow methods for scaling texture to smaller
     *     dimensions in mipmap generation and highest quality settings for S3 texture compression
     *     libsquish supports.
     *
     * png, jpg, jpeg, tga and bmp file extensions are recognised (must be lower-case).
     *
     * @param file image file.
     * @param options bit-mask to control quality and compression.
     * @param ostream stream to write texture to.
     */
    static bool build( const File& file, int options, OutputStream* ostream );

    /**
     * Create an empty instance (no OpenGL texture is created).
     */
    explicit GLTexture();

    /**
     * Load a DDS texture from a file.
     */
    explicit GLTexture( const File& file );

    /**
     * Destructor, unloads texture from GPU if loaded.
     */
    ~GLTexture();

    /**
     * Move constructor.
     */
    GLTexture( GLTexture&& t ) :
      textureId( t.textureId ), textureFormat( t.textureFormat ), textureMipmaps( t.textureMipmaps )
    {
      t.textureId      = 0;
      t.textureFormat  = 0;
      t.textureMipmaps = 0;
    }

    /**
     * Move operator.
     */
    GLTexture& operator = ( GLTexture& t )
    {
      if( &t == this ) {
        return *this;
      }

      textureId        = t.textureId;
      textureFormat    = t.textureFormat;
      textureMipmaps   = t.textureMipmaps;

      t.textureId      = 0;
      t.textureFormat  = 0;
      t.textureMipmaps = 0;

      return *this;
    }

    /**
     * OpenGL texture id.
     */
    uint id() const
    {
      return textureId;
    }

    /**
     * Return OpenGL internal texture format.
     */
    uint format() const
    {
      return textureFormat;
    }

    /**
     * True iff mipmaps have been are loaded from file.
     */
    bool hasMipmaps() const
    {
      return textureMipmaps;
    }

    /**
     * Load a DDS texture from a file.
     */
    bool load( const File& file );

    /**
     * Unload texture from GPU if loaded.
     */
    void destroy();

};

}

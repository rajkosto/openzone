/*
 *  MD2.hpp
 *
 *  MD2 model class
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Translator.hpp"

#include "client/Mesh.hpp"

namespace oz
{
namespace client
{

  class MD2
  {
    private:

      static const int MAX_VERTS = 2048;

    public:

      struct AnimInfo
      {
        int   firstFrame;
        int   lastFrame;
        float fps;
        int   repeat;
      };

      struct AnimState
      {
        Anim::Type type;
        int   repeat;

        int   startFrame;
        int   endFrame;
        int   currFrame;
        int   nextFrame;

        float fps;
        float frameTime;
        float currTime;
      };

    private:

#ifdef OZ_BUILD_TOOLS
      struct MD2Header
      {
        int id;
        int version;

        int skinWidth;
        int skinHeight;
        int frameSize;

        int nSkins;
        int nFramePositions;
        int nTexCoords;
        int nTriangles;
        int nGlCmds;
        int nFrames;

        int offSkins;
        int offTexCoords;
        int offTriangles;
        int offFrames;
        int offGLCmds;
        int offEnd;
      };

      struct MD2Vertex
      {
        ubyte p[3];
        ubyte normal;
      };

      struct MD2TexCoord
      {
        short s;
        short t;
      };

      struct MD2Frame
      {
        float     scale[3];
        float     translate[3];
        char      name[16];
        MD2Vertex verts[1];
      };

      struct MD2Triangle
      {
        short vertices[3];
        short texCoords[3];
      };

      static const Vec3 NORMALS[];
#endif

      static Vertex animBuffer[MAX_VERTS];

      int     id;

      int     nFrames;
      int     nFrameVertices;
      int     nFramePositions;

      uint    vertexTexId;
      uint    normalTexId;
      int     shaderId;

      Vertex* vertices;
      Vec4*   positions;
      Vec4*   normals;

      Mesh    mesh;

    public:

      static const AnimInfo ANIM_LIST[];

      Mat44   weaponTransf;
      bool    isLoaded;

#ifdef OZ_BUILD_TOOLS
      static void prebuild( const char* name );
#endif

      explicit MD2( int id );
      ~MD2();

      void load();

      void advance( AnimState* anim, float dt ) const;

      void drawFrame( int frame ) const;
      void draw( const AnimState* anim ) const;

  };

}
}

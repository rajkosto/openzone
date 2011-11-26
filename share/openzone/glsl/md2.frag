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

/*
 * md2.frag
 *
 * Mesh shader that reads and interpolates vertex positions from the given vertex texture.
 */

varying vec3 exPosition;
varying vec2 exTexCoord;
varying vec3 exNormal;

void main()
{
  vec3  toCamera = oz_CameraPosition - exPosition;
  vec3  normal   = normalize( exNormal );
  float dist     = length( toCamera );

  vec4 colourSample = texture2D( oz_Textures[0], exTexCoord );
  vec4 masksSample  = texture2D( oz_Textures[1], exTexCoord );

  vec4 diffuse  = skyLightColour( normal );
  vec4 specular = specularColour( masksSample.r, normal, normalize( toCamera ) );
  vec4 emission = vec4( masksSample.g, masksSample.g, masksSample.g, 0.0 );

  gl_FragData[0] = oz_Colour * colourSample * ( min( diffuse + emission, vec4( 1.0 ) ) + specular );
  gl_FragData[0] = applyFog( gl_FragData[0], dist );
}

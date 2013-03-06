--[[
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2012  Davorin Učakar
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
]]--

--[[
 * mission/test.lua
 *
 * Test world.
]]--

function onUpdate()
  ozUIShowBuild( true )
end

function onCreate()
  --ozOrbisSetGravity( -6.0 )

  ozTerraLoad( "terra" )

  ozCaelumLoad( "earth" )
  ozCaelumSetHeading( math.pi / 8 )
  ozCaelumSetPeriod( 3600 )
  ozCaelumSetTime( 0 )
  --ozCaelumSetRealTime()

--   local me = ozOrbisAddPlayer( OZ_FORCE, 137, -6, 73, OZ_SOUTH )
  --local me = ozOrbisAddPlayer( OZ_FORCE, 2000, 2002, 100, OZ_SOUTH )
--   ozCameraAddSwitchableUnit( me )
--   ozCameraSwitchTo( me )

--   init_sample()

--   ozFloraSeed()

--   init_stressTest()
end

function init_sample()
  local cX, cY, cZ = 143, 0, 79
  --local cX, cY, cZ = 2000, 2000, 100

  local lord   = ozOrbisAddObj( OZ_FORCE, "bauul",       cX - 5, cY - 11, cZ - 6, OZ_EAST )
  ozBotSetName( "Jean-Claude van Damme" )
  local memo   = ozOrbisAddObj( OZ_FORCE, "datacube",    cX - 9, cY - 6,  cZ - 6 )
  local droid  = ozOrbisAddObj( OZ_FORCE, "droid.OOM-9", cX - 3, cY - 11, cZ - 6, OZ_WEST )
  local cyborg = ozOrbisAddObj( OZ_FORCE, "cyborg",      cX - 3, cY - 13, cZ - 6, OZ_WEST )
  ozBotSetMind( "" )

  ozNirvanaAddMemo( memo, "Simulations are like miniskirts, they show a lot but hide the " ..
                          "essentials.\n\n        -- Hubert Kirrman" )
  ozNirvanaAddMemo( lord, "Buahahahaha" )

  ozCameraAddSwitchableUnit( cyborg )
  ozCameraAddSwitchableUnit( droid )

  --ozOrbisAddObj( "reaper", 145, -15, 73 )

  ozOrbisAddObj( OZ_FORCE, "musicPlayer",                  cX - 8, cY - 4, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "galileo",                      cX - 7, cY - 4, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "nvGoggles",                    cX - 6, cY - 4, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "cvicek",                       cX - 8, cY - 2, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "firstAid",                     cX - 7, cY - 2, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "binoculars",                   cX - 6, cY - 2, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "droid$hyperblaster",    cX - 8, cY - 3, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "droid$chaingun",        cX - 7, cY - 3, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "droid$grenadeLauncher", cX - 6, cY - 3, cZ - 6 )

  ozOrbisAddObj( OZ_FORCE, "key", cX + 17, cY + 3, cZ - 6 )

  ozOrbisAddObj( OZ_FORCE, "raptor",    cX + 12, cY - 30, cZ - 8 )
  ozOrbisAddObj( OZ_FORCE, "hoverTank", cX + 2,  cY - 30, cZ - 8 )

  ozOrbisAddObj( OZ_FORCE, "metalBarrel", cX + 4, cY - 11, cZ - 7 )
  ozOrbisAddObj( OZ_FORCE, "metalBarrel", cX + 4, cY - 11, cZ - 5 )
  ozOrbisAddObj( OZ_FORCE, "metalBarrel", cX + 4, cY - 11, cZ - 3 )

  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 4, cY + 11, cZ - 7 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 4, cY + 11, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 4, cY + 11, cZ - 5 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 4, cY + 11, cZ - 4 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 4, cY + 11, cZ - 3 )

  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 4, cY + 12, cZ - 7 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 4, cY + 12, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 4, cY + 12, cZ - 5 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 4, cY + 12, cZ - 4 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 4, cY + 12, cZ - 3 )

  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 5, cY + 11, cZ - 7 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 5, cY + 11, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 5, cY + 11, cZ - 5 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 5, cY + 11, cZ - 4 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 5, cY + 11, cZ - 3 )

  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 5, cY + 12, cZ - 7 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 5, cY + 12, cZ - 6 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 5, cY + 12, cZ - 5 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 5, cY + 12, cZ - 4 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX + 5, cY + 12, cZ - 3 )

  ozOrbisGenFrags( "ruins", 100, cX + 3, cY + 10, cZ, cX + 6, cY + 13, cZ, 0, 0, 2 )

  ozOrbisAddObj( OZ_FORCE, "box",         cX - 3, cY - 2,   cZ - 7 )
  ozOrbisAddObj( OZ_FORCE, "bigCrate",    cX - 3, cY - 7,   cZ - 7 )
  ozOrbisAddObj( OZ_FORCE, "metalCrate",  cX - 3, cY - 4,   cZ - 7 )
  ozOrbisAddObj( OZ_FORCE, "metalBarrel", cX - 3, cY - 5.5, cZ - 7 )

  ozOrbisAddStr( OZ_FORCE, "testCastle", cX,       cY,       cZ,      OZ_NORTH )
  ozOrbisAddStr( OZ_FORCE, "pool",       cX - 31,  cY - 7,   cZ - 9 )
  ozOrbisAddStr( OZ_FORCE, "tower",      cX - 37,  cY - 7,   cZ - 4,  OZ_WEST )
  ozOrbisAddStr( OZ_FORCE, "atlantis",   cX - 303, cY + 220, cZ - 84 )
  ozOrbisAddStr( OZ_FORCE, "house",      cX - 17,  cY - 30,  cZ - 8.5 )

  ozOrbisAddObj( OZ_FORCE, "serviceStation", cX - 33, cY - 25, cZ - 9.4, OZ_WEST )
  ozOrbisAddObj( OZ_FORCE, "console",        cX - 36, cY - 24, cZ - 11,  OZ_EAST )

  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX - 35, cY - 6, cZ + 2 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX - 35, cY - 6, cZ + 3 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX - 35, cY - 6, cZ + 4 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX - 35, cY - 8, cZ + 2 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX - 35, cY - 8, cZ + 3 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", cX - 35, cY - 8, cZ + 4 )
end

function init_stressTest()
  for i = 1, 0 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( OZ_TRY, "goblin", x, y, z + 0.75 )
    end
  end

  for i = 1, 1000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( OZ_TRY, "knight", x, y, z + 1.10 )
    end
  end

  for i = 1, 1000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( OZ_TRY, "beast", x, y, z + 1.10 )
    end
  end

  for i = 1, 10000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( OZ_TRY, "explosiveBarrel", x, y, z + 0.70 )
    end
  end

  for i = 1, 10000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( OZ_TRY, "smallCrate", x, y, z + 0.40 )
    end
  end
end
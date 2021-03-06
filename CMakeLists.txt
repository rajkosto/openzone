cmake_minimum_required(VERSION 3.0)

project(openzone)

#
# Configuration options.
#

set(OZ_VERSION 0.3.95)

option(OZ_ALLOCATOR "Overload new/delete with diagnostics and allocations tracking." OFF)
option(OZ_SIMD "Use SIMD implementation of linear algebra classes." OFF)
option(OZ_GL_ES "Use OpenGL ES 2.0 instead of OpenGL 2.1." OFF)
option(OZ_LUAJIT "Use use LuaJIT instead of official Lua." OFF)
option(OZ_NONFREE "Enable support for building S3TC textures." OFF)
option(OZ_TOOLS "Build engine tools required for game data creation." OFF)
option(OZ_TESTS "Build test programs (that should only be used by me)." OFF)

#
# Internal configuration.
#

option(OZ_BUNDLE "Adjust installation for OpenZone multi-platform bundle ZIP." OFF)
mark_as_advanced(OZ_BUNDLE)

set(OZ_PLATFORM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

if(PLATFORM_EMBEDDED)
  set(OZ_PLATFORM_NAME ${PLATFORM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
  set(OZ_GL_ES ON)
  set(OZ_NONFREE OFF)
  set(OZ_TOOLS OFF)
endif()

if(OZ_BUNDLE)
  set(OZ_PREFIX "")
  set(OZ_BINARY_SUBDIR "/${OZ_PLATFORM_NAME}")
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
else()
  set(OZ_PREFIX "${CMAKE_INSTALL_PREFIX}")
  set(OZ_BINARY_SUBDIR "")
endif()

include(cmake/PCH.cmake)
include(FindPkgConfig)

#
# Libraries.
#

# Set-up include and library directories for different toolchains.
if(ANDROID)
elseif(NACL)
  set(ENV{PKG_CONFIG_PATH} ${CMAKE_FIND_ROOT_PATH}/usr/lib/pkgconfig)
elseif(WIN32)
  set(ENV{PKG_CONFIG_PATH} ${CMAKE_FIND_ROOT_PATH}/lib/pkgconfig)
else()
  find_package(ALSA REQUIRED)
endif()

find_package(ZLIB REQUIRED)
find_package(PhysFS REQUIRED)
mark_as_advanced(PHYSFS_INCLUDE_DIR)
mark_as_advanced(PHYSFS_LIBRARY)

if(OZ_LUAJIT)
  pkg_check_modules(LUAJIT REQUIRED luajit)
  set(LUA_LIBRARIES ${LUAJIT_LIBRARIES})
  include_directories(SYSTEM ${LUAJIT_INCLUDE_DIRS})
else()
  find_package(Lua REQUIRED)
  include_directories(SYSTEM ${LUA_INCLUDE_DIR})
endif()

pkg_check_modules(SDL REQUIRED sdl2)
pkg_check_modules(SDL_TTF REQUIRED SDL2_ttf)
list(REMOVE_ITEM SDL_LIBRARIES ppapi_simple)
list(REMOVE_ITEM SDL_TTF_LIBRARIES ppapi_simple)
include_directories(SYSTEM ${SDL_INCLUDE_DIRS})

if(OZ_GL_ES)
  # On embedded platforms, GLES is part of system libraries and is guaranteed to exist in toolchain
  # or platform directory, no need to explicitly check existence or to detect library names.
  if(ANDROID)
    set(GL_LIBRARIES GLESv2)
  elseif(NACL)
    set(GL_LIBRARIES ppapi_gles2)
  else()
    pkg_check_modules(GL REQUIRED glesv2)
  endif()
else()
  find_package(OpenGL REQUIRED)
  set(GL_LIBRARIES ${OPENGL_gl_LIBRARY})
endif()

find_package(OpenAL REQUIRED)
find_package(JPEG REQUIRED)
pkg_check_modules(VORBISFILE REQUIRED vorbisfile)

# On embedded platforms, linking is mostly static, so indirect dependencies must be linked too.
if(PLATFORM_EMBEDDED)
  find_package(PNG REQUIRED)                # required by FreeType
  find_package(Freetype REQUIRED)           # required by SDL_ttf
  pkg_check_modules(OGG REQUIRED ogg)       # required by vorbisfile
  pkg_check_modules(VORBIS REQUIRED vorbis) # required by vorbisfile

  list(APPEND SDL_TTF_LIBRARIES ${FREETYPE_LIBRARIES} ${PNG_LIBRARY})
  list(APPEND VORBISFILE_LIBRARIES ${VORBIS_LIBRARIES} ${OGG_LIBRARIES})
endif()

if(OZ_TOOLS)
  pkg_check_modules(ASSIMP REQUIRED assimp)

  find_library(FREEIMAGE_LIBRARY NAMES freeimage FreeImage)
  mark_as_advanced(FREEIMAGE_LIBRARY)
  if(NOT FREEIMAGE_LIBRARY)
    message(FATAL_ERROR "FreeImage library is missing")
  endif()

  find_library(NOISE_LIBRARY noise)
  mark_as_advanced(NOISE_LIBRARY)
  if(NOT NOISE_LIBRARY)
    message(FATAL_ERROR "libnoise library is missing")
  endif()

  if(OZ_NONFREE)
    find_library(SQUISH_LIBRARY squish)
    mark_as_advanced(SQUISH_LIBRARY)

    if(NOT SQUISH_LIBRARY)
      message(FATAL_ERROR "libsquish library is missing (required by OZ_NONFREE)")
    endif()
  endif()
endif()

mark_as_advanced(MINGW32_LIBRARY)

#
# Compiler flags.
#

# Local libmad, faad and eSpeak headers.
include_directories(SYSTEM etc/include)

set(flags "-std=c++11 -pedantic -fstrict-enums -fvisibility-inlines-hidden -ffast-math")

if(BUILD_SHARED_LIBS)
  set(flags "${flags} -fPIC")
endif()

# Static analyser.
#set(flags "${flags} -Qunused-arguments --analyze")
# Address sanitizer.
#set(flags "${flags} -fsanitize=address")
# Thread sanitizer.
#set(flags "${flags} -fsanitize=thread -fpie -fPIC")
# Undefined behaviour sanitizer.
#set(flags "${flags} -fsanitize=undefined")

# Additional warnings.
if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  set(flags "${flags} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-date-time")
  set(flags "${flags} -Wno-padded -Wno-global-constructors -Wno-exit-time-destructors")
  set(flags "${flags} -Wno-shadow -Wno-switch-enum -Wno-sign-conversion -Wno-float-equal")
else()
  set(flags "${flags} -Wall -Wextra -Wcast-align -Winit-self -Wlogical-op -Wundef")
  set(flags "${flags} -Wmissing-declarations -Wnon-virtual-dtor -Woverloaded-virtual -Wformat=2")
  set(flags "${flags} -Wno-format-zero-length -Wmissing-format-attribute -Winvalid-pch")
  set(flags "${flags} -Wzero-as-null-pointer-constant -Wsuggest-attribute=noreturn")
endif()

set(CMAKE_CXX_FLAGS "${flags} ${CMAKE_CXX_FLAGS}")

# Strings for BuildInfo class.
string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" OZ_CXX_FLAGS)
string(STRIP "${CMAKE_CXX_FLAGS} ${${OZ_CXX_FLAGS}}" OZ_CXX_FLAGS)

get_directory_property(_includes INCLUDE_DIRECTORIES)
foreach(_include ${_includes})
  set(OZ_CXX_FLAGS "${OZ_CXX_FLAGS} -I${_include}")
endforeach()

get_directory_property(_defines COMPILE_DEFINITIONS)
foreach(_define ${_defines})
  set(OZ_CXX_FLAGS "${OZ_CXX_FLAGS} -D${_define}")
endforeach()

site_name(OZ_HOSTNAME)
mark_as_advanced(OZ_HOSTNAME)

#
# Sources.
#

add_subdirectory(src)

# This is to show these files in Qt Creator.
file(GLOB docFiles AUTHORS COPYING *.md)
file(GLOB_RECURSE dataFiles data/*.txt data/*.json data/*.lua data/*.vert data/*.frag)
add_custom_target(dataFiles SOURCES ${docFiles} ${dataFiles})

#
# Data files.
#

# Application launcher and icon.
if(NOT OZ_BUNDLE)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/share/applications ${CMAKE_SOURCE_DIR}/share/pixmaps
          DESTINATION share)
endif()

# Documentation, game data and libraries for a standalone bundle.
if(OZ_BUNDLE)
  file(GLOB readmes doc/*.html)
  file(GLOB dataFiles share/openzone/*.zip)

  install(FILES AUTHORS COPYING README.md ChangeLog.md TODO.md DESTINATION doc)
  install(FILES ${dataFiles} DESTINATION share/openzone)
  install(FILES ${readmes} DESTINATION .)
  install(DIRECTORY doc/licences DESTINATION .)

  install(DIRECTORY lib/${OZ_PLATFORM_NAME} DESTINATION bin USE_SOURCE_PERMISSIONS)
endif()

#
# Android project.
#

if(ANDROID)
  file(GLOB dataFiles share/openzone/*.7z)

  add_custom_command(OUTPUT ../Android
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/etc/android-project ../Android
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/share/openzone
            ../Android/assets/share/openzone)
  add_custom_target(android_project ALL DEPENDS ${CMAKE_BINARY_DIR}/../Android)
endif()

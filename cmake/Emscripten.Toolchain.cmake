include( CMakeForceCompiler )

set( EMSCRIPTEN                 ON )
set( PCH_DISABLE                ON )

set( PLATFORM_EMBEDDED          ON )
set( PLATFORM_NAME              "Emscripten" )
set( PLATFORM_TRIPLET           "emscripten" )
set( PLATFORM_PREFIX            "$ENV{EMSCRIPTEN}" )
set( PLATFORM_PORTS_PREFIX      "${CMAKE_SOURCE_DIR}/ports/Emscripten" )
set( PLATFORM_EXE_SUFFIX        ".js" )

set( CMAKE_SYSTEM_NAME          "Generic" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "JavaScript" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PORTS_PREFIX}/usr;${PLATFORM_PREFIX}/system" )
set( CMAKE_AR                   "${PLATFORM_PREFIX}/emar")
set( CMAKE_RANLIB               "${PLATFORM_PREFIX}/emranlib")
set( CMAKE_C_COMPILER           "${PLATFORM_PREFIX}/emcc")
set( CMAKE_CXX_COMPILER         "${PLATFORM_PREFIX}/em++")
set( CMAKE_C_ARCHIVE_CREATE     "${CMAKE_C_COMPILER} -o <TARGET> -emit-llvm <LINK_FLAGS> <OBJECTS>" )
set( CMAKE_CXX_ARCHIVE_CREATE   "${CMAKE_CXX_COMPILER} -o <TARGET> -emit-llvm <LINK_FLAGS> <OBJECTS>" )

cmake_force_c_compiler(         ${CMAKE_C_COMPILER} Clang )
cmake_force_cxx_compiler(       ${CMAKE_CXX_COMPILER} Clang )

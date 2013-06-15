include( CMakeForceCompiler )

set( NACL                       ON )
set( PCH_DISABLE                ON )

set( PLATFORM_EMBEDDED          ON )
set( PLATFORM_NAME              "PNaCl" )
set( PLATFORM_TRIPLET           "pnacl" )
set( PLATFORM_PREFIX            "$ENV{NACL_SDK_ROOT}/toolchain/linux_x86_pnacl/newlib" )
set( PLATFORM_PORTS_PREFIX      "${CMAKE_SOURCE_DIR}/ports/PNaCl" )
set( PLATFORM_EXE_SUFFIX        ".pexe" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "LLVM-IR" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PORTS_PREFIX};${PLATFORM_PREFIX}/usr" )
set( CMAKE_FIND_ROOT_PATH       "${CMAKE_FIND_ROOT_PATH};${PLATFORM_PREFIX}/sdk" )
set( CMAKE_AR                   "${PLATFORM_PREFIX}/bin64/${PLATFORM_TRIPLET}-ar" CACHE STRING "")
set( CMAKE_RANLIB               "${PLATFORM_PREFIX}/bin64/${PLATFORM_TRIPLET}-ranlib" CACHE STRING "")
set( CMAKE_C_COMPILER           "${PLATFORM_PREFIX}/bin64/${PLATFORM_TRIPLET}-clang" )
set( CMAKE_CXX_COMPILER         "${PLATFORM_PREFIX}/bin64/${PLATFORM_TRIPLET}-clang++" )
set( CMAKE_C_FLAGS              "-U__STRICT_ANSI__" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-U__STRICT_ANSI__" CACHE STRING "" )
set( CMAKE_C_FLAGS_RELEASE      "-O4 -ffast-math" CACHE STRING "" )
set( CMAKE_CXX_FLAGS_RELEASE    "-O4 -ffast-math" CACHE STRING "" )

cmake_force_c_compiler(         ${CMAKE_C_COMPILER} Clang )
cmake_force_cxx_compiler(       ${CMAKE_CXX_COMPILER} Clang )

set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )

include_directories( SYSTEM $ENV{NACL_SDK_ROOT}/include )

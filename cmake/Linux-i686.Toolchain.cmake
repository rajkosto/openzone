set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "i686" CACHE STRING "Target processor." )
set( CMAKE_AR                   "/usr/bin/gcc-ar" CACHE STRING "" )
set( CMAKE_NM                   "/usr/bin/gcc-nm" CACHE STRING "" )
set( CMAKE_RANLIB               "/usr/bin/gcc-ranlib" CACHE STRING "" )
set( CMAKE_C_COMPILER           "/usr/bin/gcc" )
set( CMAKE_CXX_COMPILER         "/usr/bin/g++" )
set( CMAKE_C_FLAGS              "-march=i686 -m32 -msse3 -mfpmath=sse" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "-march=i686 -m32 -msse3 -mfpmath=sse" CACHE STRING "" )
set( CMAKE_C_FLAGS_RELEASE      "-Ofast -flto" CACHE STRING "" )
set( CMAKE_CXX_FLAGS_RELEASE    "-Ofast -flto" CACHE STRING "" )

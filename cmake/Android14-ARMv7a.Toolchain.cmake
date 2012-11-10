set( ANDROID ON )

set( PLATFORM_TRIPLET           "arm-linux-androideabi" )
set( PLATFORM_PROC_PREFIX       "armeabi-v7a" )
set( PLATFORM_PREFIX            "/opt/android-ndk/platforms/android-14/arch-arm" )
set( PLATFORM_TOOL_PREFIX       "/opt/android-ndk/toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86/bin" )
set( PLATFORM_SOURCE_PREFIX     "/opt/android-ndk/sources" )
set( PLATFORM_PORTS_PREFIX      "${CMAKE_SOURCE_DIR}/ports/Android14-ARMv7a" )
set( PLATFORM_FLAGS             "-fPIC -Wno-psabi --sysroot=${PLATFORM_PREFIX}" )
set( PLATFORM_FLAGS             "${PLATFORM_FLAGS} -I${PLATFORM_SOURCE_PREFIX}/cxx-stl/gnu-libstdc++/4.6/include" )
set( PLATFORM_FLAGS             "${PLATFORM_FLAGS} -I${PLATFORM_SOURCE_PREFIX}/cxx-stl/gnu-libstdc++/4.6/libs/armeabi-v7a/include" )
set( PLATFORM_STL_LIBRARY       "${PLATFORM_SOURCE_PREFIX}/cxx-stl/gnu-libstdc++/4.6/libs/armeabi-v7a/libgnustl_static.a" )

set( CMAKE_SYSTEM_NAME          "Linux" CACHE STRING "Target system." )
set( CMAKE_SYSTEM_PROCESSOR     "ARMv7a" CACHE STRING "Target processor." )
set( CMAKE_FIND_ROOT_PATH       "${PLATFORM_PORTS_PREFIX};${PLATFORM_PREFIX}" )
set( CMAKE_C_COMPILER           "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-gcc" )
set( CMAKE_CXX_COMPILER         "${PLATFORM_TOOL_PREFIX}/${PLATFORM_TRIPLET}-g++" )
set( CMAKE_C_FLAGS              "${PLATFORM_FLAGS} -march=armv7-a -mfloat-abi=softfp -mfpu=neon" CACHE STRING "" )
set( CMAKE_CXX_FLAGS            "${PLATFORM_FLAGS} -march=armv7-a -mfloat-abi=softfp -mfpu=neon" CACHE STRING "" )
set( CMAKE_SHARED_LINKER_FLAGS  "-Wl,--fix-cortex-a8" CACHE STRING "" )

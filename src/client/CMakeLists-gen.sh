#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( client STATIC
  `echo *.{hh,cc} ui/*.{hh,cc} | sed 's/ /\n  /g' | grep -v 'openzone\.cc'` )
add_dependencies( client pch )

add_executable( openzone openzone.cc )
add_dependencies( openzone pch )
target_link_libraries( openzone client modules nirvana matrix common build_info oz \${libs_client} )

if( NACL )
  set_target_properties( openzone PROPERTIES OUTPUT_NAME "openzone.\${CMAKE_SYSTEM_PROCESSOR}.nexe" )
endif()

if( OZ_STANDALONE )
  install( TARGETS openzone RUNTIME DESTINATION bin/\${OZ_SYSTEM_NAME} )
else()
  install( TARGETS openzone RUNTIME DESTINATION bin )
endif()
EOF

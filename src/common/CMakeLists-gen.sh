#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( common STATIC
  `echo *.{hh,cc} | sed 's/ /\n  /g'` )
EOF

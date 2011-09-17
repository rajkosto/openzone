#!/bin/sh

sources='src/*/*.hpp src/*/*/*.hpp src/*/*.cpp src/*/*/*.cpp'
classes='share/openzone/class/*.rc'
output='gettext.pot'

rm -rf $output
xgettext --omit-header -c++ -s -d openzone -o $output $sources

for class in $classes; do
  class=`basename $class .rc`

  # add class name if it doesn't exist yet
  if ( grep "msgid \"$class\"" $output &> /dev/null ); then
    echo &> /dev/null;
  else
    echo >> $output
    echo "msgid \"$class\"" >> $output
    echo "msgstr \"\"" >> $output
  fi
done

# extract weapon names
weapons="`grep -h 'weapon[0-9][0-9]\.name' $classes | sed -e 's/.*"\(.*\)"/\1/'`"

for weapon in $weapons; do
  # add weapon name if it doesn't exist yet
  if ( grep "msgid \"$weapon\"" $output &> /dev/null ); then
    echo &> /dev/null;
  else
    echo >> $output
    echo "msgid \"$weapon\"" >> $output
    echo "msgstr \"\"" >> $output
  fi
done
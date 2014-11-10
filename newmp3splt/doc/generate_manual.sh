#!/bin/bash


script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

MANUAL_FILE=manual.html

man2html ./mp3splt.1 > $MANUAL_FILE

sed -i 's/^Content-type: text\/html.*$//' $MANUAL_FILE
sed -i 's/^Section: User Commands  (1)<BR><A HREF="#index">Index<\/A>//' $MANUAL_FILE
sed -i 's/^<A HREF="\/cgi-bin\/man\/man2html">Return to Main Contents<\/A>//' $MANUAL_FILE

sed -i 's/<A HREF="cgi:\/\/.*\?">\(.*\?\)<\/A>/\1/' $MANUAL_FILE
sed -i 's/<A HREF="mailto:a+-+@n+-+">a+-+@n+-+<\/A>/a+-+@n+-+/' $MANUAL_FILE
sed -i 's/<B><A HREF=".*\?man2html.*\?mp3wrap.*\?<\/A><\/B>(1)$/<b><a href="http:\/\/mp3wrap.sourceforge.net\/">mp3wrap<\/a><\/b>/' $MANUAL_FILE

sed -i 's/<A HREF="\/cgi-bin\/man\/man2html">man2html<\/A>,/man2html/' $MANUAL_FILE

sed -i '/^Offset visualization:/,+10d' $MANUAL_FILE


#!/bin/bash

#this script is used to update the version of the package

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ./include_variables.sh "quiet_noflags" || exit 1

#the program version, that we have in the $LIBMP3SPLT_VERSION variable
VERSION=$LIBMP3SPLT_VERSION
PROGRAM="libmp3splt"

sed -i "1,4s/libmp3splt (\(.*\))/libmp3splt ($VERSION)/" ./debian/changelog || exit 1

#README
#./README:       libmp3splt version 0.3.1
sed -i "s/\s*$PROGRAM version.*/\t$PROGRAM version $VERSION/" README || exit 1

versions=(${VERSION//./ })
major_version=${versions[0]}
minor_version=${versions[1]}
if [ -z "${versions[3]}" ];then
 micro_version=${versions[2]}
else
 micro_version=${versions[2]}"."${versions[3]}
fi

sed -i "s/libmp3splt_major_version\], \[.*\])/libmp3splt_major_version\], \[$major_version\])/" ./configure.ac
sed -i "s/libmp3splt_minor_version\], \[.*\])/libmp3splt_minor_version\], \[$minor_version\])/" ./configure.ac
sed -i "s/libmp3splt_micro_version\], \[.*\])/libmp3splt_micro_version\], \[$micro_version\])/" ./configure.ac

echo "Finished setting up $PROGRAM to version $VERSION."


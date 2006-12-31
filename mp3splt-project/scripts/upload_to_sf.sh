#!/bin/bash
#usage = ./upload_to_sf.sh DIR
#-uploads DIR/* to sf.net

############# uploading to sourceforge.net #####
#we put the files on the sourceforge ftp
function upload_to_sourceforge()
{
  DIR=$1;
  echo;
  echo "Uploading files to sourceforge.net...";
  echo;

  for a in $DIR/*; do
    lftp -e "cd /incoming;put $DIST_VERSION/$a;quit" -u anonymous,\
             upload.sourceforge.net || exit 1;
  done;
}
############# finish uploading to sourceforge.net #####

################################
############# main program #####
################################

if [[ $# != 1 ]];then
  echo "Usage : $0 DIRECTORY_WITH_FILES_TO_UPLOAD";
  exit 1;
fi

DIRECTORY=$1;
upload_to_sourceforge $DIRECTORY;

#!/bin/sh
echo "This script must not be run (see its content)"
exit 1

#at the end of the /etc/rc file :
########### mp3splt-project packages

rm -rf /etc/make_packages

echo "I will compile the packages in 5 seconds..."
echo "Press Enter to cancel"
touch /etc/make_packages
/etc/rc.mp3splt&
read NAME
rm -rf /etc/make_packages

########### end mp3splt-project packages

#rc.mp3splt :
#!/usr/local/bin/bash
#make openbsd packages
make_packages()
{
      export PATH=$PATH:/usr/local/bin:/usr/bin
      echo
      echo "Making OpenBSD packages...";
      echo
      cd /root/progs_src/ &&\
	  rm -rf ./mp3splt-project &&\
          scp -P 4422 -r ion@10.0.2.2:/mnt/personal/hacking/mp3splt/mp3splt-project . &&\
	  cd mp3splt-project && make openbsd_packages &&\
	  scp -P 4422 -r *obsd*.tgz \
	  ion@10.0.2.2:/mnt/personal/hacking/mp3splt/mp3splt-project &&\
	  rm *obsd*.tgz &&\
	  halt -p || exit 1
      exit 0
}

sleep 5
if [[ -e /etc/make_packages ]];then
    make_packages
fi

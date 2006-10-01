#!/bin/bash

################# variables to set ############

#program versions
LIBMP3SPLT_REAL_VERSION=0.4_rc1
MP3SPLT_REAL_VERSION=2.2_rc1
MP3SPLT_GTK_REAL_VERSION=0.4_rc1

#if we upload to sourceforge or not
UPLOAD_TO_SOURCEFORGE=0
################## end variables to set ############

################## confirmation question ############
#the confirmation question
echo
echo "This script is used by the developers to auto-create packages for releases"
echo "!!!! Warning !!!! This script may be dangerous and erase data on your computer !!"
echo "Please remember that you are using the script at your own risk !"
echo
sleep 3

select=("I know what I'm doing and I use it at my own risk" "Quit")
select continue in "${select[@]}";do
    if [[ $continue = "Quit" ]]; then
        exit 0
    else
        break
    fi
done

#don't run the script as root
if [[ `id -u` == 0 ]]; then
    echo "The script must not be run as root"
    exit 1
fi

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROJECT_DIR=$script_dir/..

#we include the package variables
. ./libmp3splt/include_variables.sh "quiet"
. ./newmp3splt/include_variables.sh "quiet"
. ./mp3splt-gtk/include_variables.sh "quiet"

cd $PROJECT_DIR
################## end confirmation question ############

DATE_START=`date`
################## update versions ############
echo
echo "Updating versions..."
echo
sleep 2

#we update the real versions
#libmp3splt
sed -i "s/LIBMP3SPLT_VERSION=.*/LIBMP3SPLT_VERSION=$LIBMP3SPLT_REAL_VERSION/"\
 ./libmp3splt/include_variables.sh
#mp3splt + its libmp3splt library
sed -i "s/MP3SPLT_VERSION=.*/MP3SPLT_VERSION=$MP3SPLT_REAL_VERSION/"\
 ./libmp3splt/include_variables.sh
sed -i "s/LIBMP3SPLT_VERSION=.*/LIBMP3SPLT_VERSION=$LIBMP3SPLT_REAL_VERSION/"\
 ./libmp3splt/include_variables.sh
#mp3splt-gtk + its libmp3splt library
sed -i "s/MP3SPLT_GTK_VERSION=.*/MP3SPLT_GTK_VERSION=$MP3SPLT_GTK_REAL_VERSION/"\
 ./mp3splt-gtk/include_variables.sh
sed -i "s/LIBMP3SPLT_VERSION=.*/LIBMP3SPLT_VERSION=$LIBMP3SPLT_REAL_VERSION/"\
 ./libmp3splt/include_variables.sh

#we update the versions
./libmp3splt/update_version.sh
./newmp3splt/update_version.sh
./mp3splt-gtk/update_version.sh

cd $PROJECT_DIR
################## end update versions ############

#puts .sarge or .etch version
#$1=.sarge or $1=.etch
function put_debian_version()
{
    sed -i "1,4s/libmp3splt (\(.*\))/libmp3splt (\1.$1)/" libmp3splt/debian/changelog
    sed -i "1,4s/mp3splt (\(.*\))/mp3splt (\1.$1)/" newmp3splt/debian/changelog
    sed -i "1,4s/mp3splt-gtk (\(.*\))/mp3splt-gtk (\1.$1)/" mp3splt-gtk/debian/changelog
}

#cleans .sarge or .etch
#$1=.sarge or $1=.etch
function clean_debian_version()
{
    sed -i "1,4s/libmp3splt (\(.*\).$1)/libmp3splt (\1)/" libmp3splt/debian/changelog
    sed -i "1,4s/mp3splt (\(.*\).$1)/mp3splt (\1)/" newmp3splt/debian/changelog
    sed -i "1,4s/mp3splt-gtk (\(.*\).$1)/mp3splt-gtk (\1)/" mp3splt-gtk/debian/changelog
}

#make the chroot debian flavors
#example : make_debian_flavor "ubuntu" "breezy"
#example : make_debian_flavor "debian" "sarge"
function make_debian_flavor()
{
    echo
    echo "Creating $ARCH $1 $2 packages..."
    echo
    sleep 2
    
    put_debian_version "$2"
    dchroot -d -c $1_$2 "export LC_ALL=\"C\" && make debian_packages " || exit 1
    clean_debian_version "$2"
    cd $PROJECT_DIR
}

############# source packages ################
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating source distribution..."
    echo
    sleep 2
    
    make source_packages
fi
############# end source packages ################

############# debian packages ################
echo
echo "Creating $ARCH debian packages..."
echo
sleep 2

put_debian_version "etch"
make debian_packages || exit 1
clean_debian_version "etch"

make_debian_flavor "debian" "sarge"
make_debian_flavor "debian" "sid"
cd $PROJECT_DIR
############# end debian packages ################

############# ubuntu packages ##########################
echo
echo "Creating $ARCH ubuntu packages..."
echo
sleep 2

make_debian_flavor "ubuntu" "breezy"
make_debian_flavor "ubuntu" "dapper"
make_debian_flavor "ubuntu" "edgy"
cd $PROJECT_DIR
############# end ubuntu packages ##########################

############# gnu/linux static build #####
echo
echo "Creating $ARCH gnu/linux static builds..."
echo
sleep 2

make static_packages || exit 1
cd $PROJECT_DIR
############# end gnu/linux static build #####

############# gnu/linux dynamic build #####
echo
echo "Creating $ARCH dynamic gnu/linux builds..."
echo
sleep 2

make dynamic_packages || exit 1
cd $PROJECT_DIR
############# end gnu/linux dynamic build #####

############# gentoo ebuilds ################
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating gentoo ebuilds..."
    echo
    sleep 2
    
    dchroot -d -c gentoo "make gentoo_ebuilds" || exit 1
    cd $PROJECT_DIR
fi
############# end gentoo ebuilds ################

############# windows installers ################
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating $ARCH windows installers..."
    echo
    sleep 2
    
    make windows_cross_installers || exit 1
    cd $PROJECT_DIR
fi
############# end windows installers ################

############# RPM packages creation ################
echo
echo "Creating $ARCH RPMs..."
echo
sleep 2

make rpm_packages || exit 1
cd $PROJECT_DIR
############# end RPM packages creation ################

############# archlinux packages #########
echo
echo "Creating $ARCH archlinux packages..."
echo
sleep 2

dchroot -d -c arch "make arch_packages" || exit 1
cd $PROJECT_DIR
############# end archlinux packages #########

############# slackware packages #########
echo
echo "Creating $ARCH slackware packages..."
echo
sleep 2

dchroot -d -c slackware "make slackware_fakeroot_packages";
cd $PROJECT_DIR
############# end slackware packages #####

############# amd64 packages #########
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating amd64 packages..."
    echo
    sleep 2
    
    #we change the architecture
    
    
    #cd /mnt/personal/systems/debian_amd64 && ./debian_amd64
    cd $PROJECT_DIR
fi
############# end amd64 packages #########

DATE_END=`date`
echo
echo "Start date : "$DATE_START
echo "End date : "$DATE_END
echo
exit 0

############# openbsd packages #####
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating $ARCH openbsd packages..."
    echo
    sleep 2
    
    #cd /mnt/personal/systems/bsd-based/openbsd && ./openbsd
    cd $PROJECT_DIR
fi
############# end openbsd packages #####

############# netbsd packages #####
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating $ARCH netbsd packages..."
    echo
    sleep 2
    
    #cd /mnt/personal/systems/bsd-based/netbsd && ./netbsd
    cd $PROJECT_DIR
fi
############# end netbsd packages #####

############# freebsd packages #####
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating $ARCH freebsd packages..."
    echo
    sleep 2
    
    #cd /mnt/personal/systems/bsd-based/freebsd && ./freebsd
    cd $PROJECT_DIR
fi
############# end freebsd packages #####

############# nexenta gnu/opensolaris packages #####
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating $ARCH nexenta gnu/opensolaris packages..."
    echo
    sleep 2
    
    cd /mnt/personal/systems/opensolaris/ && ./nexenta
    cd $PROJECT_DIR
fi
############# end nexenta gnu/opensolaris packages #####

############# finish packaging #####
if [[ $ARCH = "i386" ]];then
    echo
    echo "Finishing packaging..."
    echo
    sleep 2
    
    #copy packages to the new directory
    #the new release directory
    RELEASE_DIR=release_$LIBMP3SPLT_VERSION
    
    mkdir -p $RELEASE_DIR
    rm -rf $RELEASE_DIR/*
    
    #debian
    mv ./*sarge_i386.deb ./$RELEASE_DIR || exit 1
    mv ./*etch_i386.deb ./$RELEASE_DIR || exit 1
    mv ./*sid_i386.deb ./$RELEASE_DIR || exit 1
    #ubuntu
    mv ./*breezy_i386.deb ./$RELEASE_DIR || exit 1
    mv ./*dapper_i386.deb ./$RELEASE_DIR || exit 1
    mv ./*edgy_i386.deb ./$RELEASE_DIR || exit 1
    #nexenta
    mv ./*solaris_i386.deb ./$RELEASE_DIR || exit 1
    #windows
    mv ./*_i386.exe ./$RELEASE_DIR || exit 1
    #openbsd
    #arch ?
    mv ./*obsd*.tgz ./$RELEASE_DIR || exit 1
    #netbsd
    #arch ?
    mv ./*nbsd*.tgz ./$RELEASE_DIR || exit 1
    #freebsd
    #arch ?
    mv ./*fbsd*.tbz ./$RELEASE_DIR || exit 1
    #gnu/linux static+dynamic
    mv ./*_static_i386.tar.gz ./$RELEASE_DIR || exit 1
    mv ./*_dynamic_i386.tar.gz ./$RELEASE_DIR || exit 1
    #arch linux
    mv ./*i686.pkg.tar.gz ./$RELEASE_DIR || exit 1
    #gentoo ebuilds
    mv ./*ebuild.tar.gz ./$RELEASE_DIR || exit 1
    #source code
    mv ./*.tar.gz ./$RELEASE_DIR || exit 1
    #rpms
    mv ./*.src.rpm ./$RELEASE_DIR || exit 1
    mv ./*i386.rpm ./$RELEASE_DIR || exit 1
    #slackware
    mv ./*i386.tgz ./$RELEASE_DIR || exit 1
fi
############# end finish packaging #####

############# uploading to sourceforge.net #####
#we put the files on the sourceforge ftp
if [[ $ARCH = "i386" ]];then
    if [[ $UPLOAD_TO_SOURCEFORGE == 1 ]]; then
        echo
        echo "Uploading files to sourceforge.net..."
        echo
        sleep 2
        for a in ls $DIST_VERSION; do
            lftp -e "cd /incoming;put $DIST_VERSION/$a;quit" -u anonymous,\
                upload.sourceforge.net || exit 1
        done
    fi
fi
############# finish uploading to sourceforge.net #####

if [[ $ARCH = "i386" ]];then
    echo
    echo "The packaging is finished :)"
    echo
    
    DATE_END=`date`
    
    echo
    echo "Start date : "$DATE_START
    echo "End date : "$DATE_END
    echo
fi

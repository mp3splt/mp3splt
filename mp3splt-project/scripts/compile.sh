#!/bin/bash

################# variables to set ############

#program directories
LIBMP3SPLT_DIR=libmp3splt
MP3SPLT_DIR=newmp3splt
MP3SPLT_GTK_DIR=mp3splt-gtk

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
echo "You should modify this script in order to use it on your computer."
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
. ./${LIBMP3SPLT_DIR}/include_variables.sh "quiet"
. ./${MP3SPLT_DIR}/include_variables.sh "quiet"
. ./${MP3SPLT_GTK_DIR}/include_variables.sh "quiet"

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
 ./${LIBMP3SPLT_DIR}/include_variables.sh
#mp3splt + its libmp3splt library
sed -i "s/MP3SPLT_VERSION=.*/MP3SPLT_VERSION=$MP3SPLT_REAL_VERSION/"\
 ./${LIBMP3SPLT_DIR}/include_variables.sh
sed -i "s/LIBMP3SPLT_VERSION=.*/LIBMP3SPLT_VERSION=$LIBMP3SPLT_REAL_VERSION/"\
 ./${LIBMP3SPLT_DIR}/include_variables.sh
#mp3splt-gtk + its libmp3splt library
sed -i "s/MP3SPLT_GTK_VERSION=.*/MP3SPLT_GTK_VERSION=$MP3SPLT_GTK_REAL_VERSION/"\
 ./${MP3SPLT_GTK_DIR}/include_variables.sh
sed -i "s/LIBMP3SPLT_VERSION=.*/LIBMP3SPLT_VERSION=$LIBMP3SPLT_REAL_VERSION/"\
 ./${LIBMP3SPLT_DIR}/include_variables.sh

#we update the versions
./${LIBMP3SPLT_DIR}/update_version.sh
./${MP3SPLT_DIR}/update_version.sh
./${MP3SPLT_GTK_DIR}/update_version.sh

cd $PROJECT_DIR
################## end update versions ############

#puts .sarge or .etch version
#$1=.sarge or $1=.etch
function put_debian_version()
{
    sed -i "1,4s/libmp3splt (\(.*\))/libmp3splt (\1.$1)/" ${LIBMP3SPLT_DIR}/debian/changelog
    sed -i "1,4s/mp3splt (\(.*\))/mp3splt (\1.$1)/" ${MP3SPLT_DIR}/debian/changelog
    sed -i "1,4s/mp3splt-gtk (\(.*\))/mp3splt-gtk (\1.$1)/" ${MP3SPLT_GTK_DIR}/debian/changelog
}

#cleans .sarge or .etch
#$1=.sarge or $1=.etch
function clean_debian_version()
{
    sed -i "1,4s/libmp3splt (\(.*\).$1)/libmp3splt (\1)/" ${LIBMP3SPLT_DIR}/debian/changelog
    sed -i "1,4s/mp3splt (\(.*\).$1)/mp3splt (\1)/" ${MP3SPLT_DIR}/debian/changelog
    sed -i "1,4s/mp3splt-gtk (\(.*\).$1)/mp3splt-gtk (\1)/" ${MP3SPLT_GTK_DIR}/debian/changelog
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
    
    make source_packages || exit 1
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

#there is no sarge for x86_64
if [[ $ARCH = "i386" ]];then
    make_debian_flavor "debian" "sarge"
fi

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

dchroot -d -c slackware "make slackware_fakeroot_packages" || exit 1
cd $PROJECT_DIR
############# end slackware packages #####

############# amd64 packages #########
#TODO
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating amd64 packages..."
    echo
    sleep 2
    
    #cd /mnt/personal/systems/debian_amd64 && ./debian_amd64
    cd $PROJECT_DIR
fi
############# end amd64 packages #########

############# openbsd packages #####
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating $ARCH openbsd packages..."
    echo
    sleep 2
    
    cd /mnt/personal/systems/bsd-based/openbsd && ./openbsd || exit 1
    cd $PROJECT_DIR
fi
############# end openbsd packages #####

############# netbsd packages #####
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating $ARCH netbsd packages..."
    echo
    sleep 2
    
    cd /mnt/personal/systems/bsd-based/netbsd && ./netbsd || exit 1
    cd $PROJECT_DIR
fi
############# end netbsd packages #####

############# freebsd packages #####
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating $ARCH freebsd packages..."
    echo
    sleep 2
    
    cd /mnt/personal/systems/bsd-based/freebsd && ./freebsd || exit 1
    cd $PROJECT_DIR
fi
############# end freebsd packages #####

############# nexenta gnu/opensolaris packages #####
if [[ $ARCH = "i386" ]];then
    echo
    echo "Creating $ARCH nexenta gnu/opensolaris packages..."
    echo
    sleep 2
    
    cd /mnt/personal/systems/opensolaris/ && ./nexenta || exit 1
    cd $PROJECT_DIR
fi
############# end nexenta gnu/opensolaris packages #####

############# cleaning the distribution #####
if [[ $ARCH = "i386" ]];then
    make distclean || exit 1
fi
############# end cleaning the distribution #####

############# finish packaging #####
if [[ $ARCH = "i386" ]];then
    echo
    echo "Finishing packaging..."
    echo
    sleep 2
    
    #copy packages to the new directory
    #the new release directory
    RELEASE_DIR=release_$LIBMP3SPLT_VERSION
    
    #backup existing directory
    DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
    if [[ -e $RELEASE_DIR ]];then
        mv $RELEASE_DIR ${RELEASE_DIR}${DATEMV}
    fi
    mkdir -p $RELEASE_DIR
    
    ##i386
    #debian
    mv ./*sarge_i386.deb ./$RELEASE_DIR || echo "sarge i386 warning"
    mv ./*etch_i386.deb ./$RELEASE_DIR || echo "etch i386 warning"
    mv ./*sid_i386.deb ./$RELEASE_DIR || echo "sid i386 warning"
    #ubuntu
    mv ./*breezy_i386.deb ./$RELEASE_DIR || echo "breezy i386 warning"
    mv ./*dapper_i386.deb ./$RELEASE_DIR || echo "dapper i386 warning"
    mv ./*edgy_i386.deb ./$RELEASE_DIR || echo "edgy i386 warning"
    #nexenta
    mv ./*solaris-i386.deb ./$RELEASE_DIR || echo "solaris i386 warning"
    #windows
    mv ./*_i386.exe ./$RELEASE_DIR || echo "windows i386 warning"
    #openbsd
    mv ./*obsd*i386*.tgz ./$RELEASE_DIR || echo "openbsd i386 warning"
    #netbsd
    mv ./*nbsd*i386*.tgz ./$RELEASE_DIR || echo "netbsd i386 warning"
    #freebsd
    mv ./*fbsd*i386*.tbz ./$RELEASE_DIR || echo "freebsd i386 warning"
    #gnu/linux static+dynamic
    mv ./*_static_i386.tar.gz ./$RELEASE_DIR || echo "static i386 warning"
    mv ./*_dynamic_i386.tar.gz ./$RELEASE_DIR || echo "dynamic i386 warning"
    #arch linux
    mv ./*i686.pkg.tar.gz ./$RELEASE_DIR || echo "arch i686 warning"
    #gentoo ebuilds
    mv ./*ebuild.tar.gz ./$RELEASE_DIR || echo "ebuild warning"
    #i386 rpms
    mv ./*i386.rpm ./$RELEASE_DIR || echo "rpm i386 warning"
    #slackware
    mv ./*i386.tgz ./$RELEASE_DIR || echo "slackware i386 warning"
    
    ##source
    #source code
    mv ./*.tar.gz ./$RELEASE_DIR || echo "source code warning"
    #source rpms
    mv ./*.src.rpm ./$RELEASE_DIR || echo "source code rpm warning"
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

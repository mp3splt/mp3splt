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

#print functions
function print_red()
{
    echo -e "\033[1;31m${@}\033[0m"
}
function print_yellow()
{
    echo -e "\033[1;33m${@}\033[0m"
}
function print_cyan()
{
    echo -e "\033[36m${@}\033[0m"
}
function print_green()
{
    echo -e "\033[1;32m${@}\033[0m"
}

################## confirmation question ############
#the confirmation question
function confirmation_question()
{
    echo
    print_red "This script is used by the developers to auto-create packages for releases"
    print_red "You should modify this script in order to use it on your computer."
    print_red "Please remember that you are using the script at your own risk !"
    echo
    
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
        print_red "The script must not be run as root"
        exit 1
    fi    
}
################## end confirmation question ############

################## update versions ############
function update_versions()
{
    if [[ $ARCH = "i386" ]];then
        echo
        print_yellow "Updating versions..."
        echo
        
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
        ./${LIBMP3SPLT_DIR}/update_version.sh || exit 1
        ./${MP3SPLT_DIR}/update_version.sh || exit 1
        ./${MP3SPLT_GTK_DIR}/update_version.sh || exit 1
        
        cd $PROJECT_DIR
    fi
}
################## end update versions ############

############# source packages ################
function source_packages()
{
    if [[ $ARCH = "i386" ]];then
        echo
        print_yellow "Creating source distribution..."
        
        make -s source_packages || exit 1
    fi
}
############# end source packages ################

############# debian packages ################
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
    print_yellow "Creating $ARCH $1 $2 packages..."
    echo
    
    #if we don't have the distribution files
    if [[ ! -f "mp3splt-gtk-${MP3SPLT_GTK_REAL_VERSION}.${2}_${ARCH}.deb" ||
          ! -f "mp3splt-${MP3SPLT_REAL_VERSION}.${2}_${ARCH}.deb" ||
          ! -f "libmp3splt-${LIBMP3SPLT_REAL_VERSION}.${2}_${ARCH}.deb" ]];then
        
        put_debian_version "$2"
        dchroot -d -c $1_$2 "export LC_ALL=\"C\" && make -s debian_packages " || exit 1
        clean_debian_version "$2"
        cd $PROJECT_DIR
    fi
}

function debian_packages()
{
    echo
    print_yellow "Creating $ARCH debian packages..."
    
    put_debian_version "etch"
    make -s debian_packages || exit 1
    clean_debian_version "etch"
    
    #there is no sarge for x86_64
    if [[ $ARCH = "i386" ]];then
        make_debian_flavor "debian" "sarge"
    fi
    
    make_debian_flavor "debian" "sid"
    cd $PROJECT_DIR
}
############# end debian packages ################

############# ubuntu packages ##########################
function ubuntu_packages()
{
    #echo
    #print_yellow "Creating $ARCH ubuntu packages..."
    #echo
    
    make_debian_flavor "ubuntu" "dapper"
    make_debian_flavor "ubuntu" "edgy"
    cd $PROJECT_DIR
}
############# end ubuntu packages ##########################

############# gnu/linux static build #####
function gnu_linux_static_packages()
{
    echo
    print_yellow "Creating $ARCH gnu/linux static builds..."
    
    make -s static_packages || exit 1
    cd $PROJECT_DIR
}
############# end gnu/linux static build #####

############# gnu/linux dynamic build #####
function gnu_linux_dynamic_packages()
{
    echo
    print_yellow "Creating $ARCH dynamic gnu/linux builds..."
    
    make -s dynamic_packages || exit 1
    cd $PROJECT_DIR
}
############# end gnu/linux dynamic build #####

############# gentoo ebuilds ################
function gentoo_packages()
{
    if [[ $ARCH = "i386" ]];then
        echo
        print_yellow "Creating gentoo ebuilds..."
        echo
        
        dchroot -d -c gentoo "make -s gentoo_ebuilds" || exit 1
        cd $PROJECT_DIR
    fi
}
############# end gentoo ebuilds ################

############# windows installers ################
function windows_cross_installers()
{
    if [[ $ARCH = "i386" ]];then
        echo
        print_yellow "Creating $ARCH windows installers..."
        
        #if we don't have the distribution file
        DIST_FILE1="./mp3splt_${MP3SPLT_VERSION}_${ARCH}.exe"
        DIST_FILE2="./mp3splt-gtk_${MP3SPLT_GTK_VERSION}_${ARCH}.exe"
        if [[ ! -f $DIST_FILE1 || ! -f $DIST_FILE2 ]];then
            make -s windows_cross_installers || exit 1
        else
            echo
            print_cyan "We already have the $DIST_FILE1 distribution file"
            print_cyan "and the $DIST_FILE2 distribution file !"
        fi
        cd $PROJECT_DIR
    fi
}
############# end windows installers ################

############# RPM packages creation ################
function rpm_packages()
{
    echo
    print_yellow "Creating $ARCH RPMs..."
    
    make -s rpm_packages || exit 1
    cd $PROJECT_DIR
}
############# end RPM packages creation ################

############# archlinux packages #########
function archlinux_packages()
{
    echo
    print_yellow "Creating $ARCH archlinux packages..."
    echo
    
    dchroot -d -c arch "make -s arch_packages" || exit 1
    cd $PROJECT_DIR
}
############# end archlinux packages #########

############# slackware packages #########
function slackware_packages()
{
    echo
    print_yellow "Creating $ARCH slackware packages..."
    echo
    
    dchroot -d -c slackware "make -s slackware_fakeroot_packages" || exit 1
    cd $PROJECT_DIR
}
############# end slackware packages #####

############# amd64 packages #########
function amd64_packages()
{
    #TODO
    if [[ $ARCH = "i386" ]];then
        echo
        print_yellow "Creating amd64 packages..."
        
        #cd /mnt/personal/systems/debian_amd64 && ./debian_amd64
        cd $PROJECT_DIR
    fi
}
############# end amd64 packages #########

############# openbsd packages #####
function openbsd_packages()
{
    if [[ $ARCH = "i386" ]];then
        echo
        print_yellow "Creating $ARCH openbsd packages..."
        
        #if we don't have the distribution file
        DIST_FILE1="libmp3splt_obsd_${ARCH}-${LIBMP3SPLT_VERSION}.tgz"
        DIST_FILE2="mp3splt_obsd_${ARCH}-${MP3SPLT_VERSION}.tgz"
        DIST_FILE3="mp3splt-gtk_obsd_${ARCH}-${MP3SPLT_GTK_VERSION}.tgz"
        #if the 3 files do not exist
        if ! [[ -f $DIST_FILE1 || -f $DIST_FILE2 || -f $DIST_FILE3 ]];then
            cd /mnt/personal/systems/bsd-based/openbsd && ./openbsd || exit 1
            cd $PROJECT_DIR
        else
            echo
            print_cyan "We already have the OpenBSD packages !"
        fi
    fi
}
############# end openbsd packages #####

############# netbsd packages #####
function netbsd_packages()
{
    if [[ $ARCH = "i386" ]];then
        echo
        print_yellow "Creating $ARCH netbsd packages..."
        
        #if we don't have the distribution file
        DIST_FILE1="libmp3splt_nbsd_${ARCH}-${LIBMP3SPLT_VERSION}.tgz"
        DIST_FILE2="mp3splt_nbsd_${ARCH}-${MP3SPLT_VERSION}.tgz"
        DIST_FILE3="mp3splt-gtk_nbsd_${ARCH}-${MP3SPLT_GTK_VERSION}.tgz"
        if [[ ! -f $DIST_FILE1 || ! -f $DIST_FILE2 || ! -f $DIST_FILE3 ]];then
            cd /mnt/personal/systems/bsd-based/netbsd && ./netbsd || exit 1
            cd $PROJECT_DIR
        else
            echo
            print_cyan "We already have the NetBSD packages !"
        fi
    fi
}
############# end netbsd packages #####

############# freebsd packages #####
function freebsd_packages
{
    if [[ $ARCH = "i386" ]];then
        echo
        print_yellow "Creating $ARCH freebsd packages..."
        
        #we change 2.2_rc1 to 2.2.r1
        TEMP_MP3SPLT_VERSION=${MP3SPLT_VERSION/_/.}
        NEW_MP3SPLT_VERSION=${TEMP_MP3SPLT_VERSION/rc/r}
        #we change 0.4_rc1 to 0.4.r1
        TEMP_LIBMP3SPLT_VERSION=${LIBMP3SPLT_VERSION/_/.}
        NEW_LIBMP3SPLT_VERSION=${TEMP_LIBMP3SPLT_VERSION/rc/r}
        #we change 0.4_rc1 to 0.4.r1
        TEMP_MP3SPLT_GTK_VERSION=${MP3SPLT_GTK_VERSION/_/.}
        NEW_MP3SPLT_GTK_VERSION=${TEMP_MP3SPLT_GTK_VERSION/rc/r}
        
        #if we don't have the distribution file
        DIST_FILE1="./libmp3splt_fbsd_${ARCH}-${NEW_LIBMP3SPLT_VERSION}.tbz"
        DIST_FILE2="./mp3splt_fbsd_${ARCH}-${NEW_MP3SPLT_VERSION}.tbz"
        DIST_FILE3="./mp3splt-gtk_fbsd_${ARCH}-${NEW_MP3SPLT_GTK_VERSION}.tbz"
        if [[ ! -f $DIST_FILE1 || ! -f $DIST_FILE2 || ! -f $DIST_FILE3 ]];then
            cd /mnt/personal/systems/bsd-based/freebsd && ./freebsd || exit 1
            cd $PROJECT_DIR
        else
            echo
            print_cyan "We already have the FreeBSD packages !"
        fi
    fi
}
############# end freebsd packages #####

############# nexenta gnu/opensolaris packages #####
function nexenta_packages()
{
    if [[ $ARCH = "i386" ]];then
        echo
        print_yellow "Creating $ARCH nexenta gnu/opensolaris packages..."
        echo
        
        #if we don't have the distribution file
        DIST_FILE1="./libmp3splt_${LIBMP3SPLT_VERSION}_solaris-${ARCH}.deb"
        DIST_FILE2="./mp3splt_${MP3SPLT_VERSION}_solaris-${ARCH}.deb"
        DIST_FILE3="./mp3splt-gtk_${MP3SPLT_GTK_VERSION}_solaris-${ARCH}.deb"
        if [[ ! -f $DIST_FILE1 || ! -f $DIST_FILE2 || ! -f $DIST_FILE3 ]];then
            cd /mnt/personal/systems/opensolaris/ && ./nexenta || exit 1
            cd $PROJECT_DIR
        else
            echo
            print_cyan "We already have the Nexenta packages !"
        fi
    fi
}
############# end nexenta gnu/opensolaris packages #####

############# finish packaging #####
function finish_packaging()
{
    if [[ $ARCH = "i386" ]];then
        echo
        print_yellow "Finishing packaging..."
        echo
        
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
        mv ./*sarge_i386.deb ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) sarge i386 is(are) missing"
        mv ./*etch_i386.deb ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) etch i386 is(are) missing"
        mv ./*sid_i386.deb ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) sid i386 is(are) missing"
        #ubuntu
        mv ./*dapper_i386.deb ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) dapper i386 is(are) missing"
        mv ./*edgy_i386.deb ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) edgy i386 is(are) missing"
        #nexenta
        mv ./*solaris-i386.deb ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) solaris i386 is(are) missing"
        #windows
        mv ./*_i386.exe ./$RELEASE_DIR 2>/dev/null 2>/dev/null ||\
            print_red "Warning: package(s) windows i386 is(are) missing"
        #openbsd
        mv ./*obsd*i386*.tgz ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) openbsd i386 is(are) missing"
        #netbsd
        mv ./*nbsd*i386*.tgz ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) netbsd i386 is(are) missing"
        #freebsd
        mv ./*fbsd*i386*.tbz ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) freebsd i386 is(are) missing"
        #gnu/linux static+dynamic
        mv ./*_static_i386.tar.gz ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) static i386 is(are) missing"
        mv ./*_dynamic_i386.tar.gz ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) dynamic i386 is(are) missing"
        #arch linux
        mv ./*i686.pkg.tar.gz ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) arch i686 is(are) missing"
        #gentoo ebuilds
        mv ./*ebuild.tar.gz ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) ebuild is(are) missing"
        #i386 rpms
        mv ./*i386.rpm ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) rpm i386 is(are) missing"
        #slackware
        mv ./*i386.tgz ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) slackware i386 is(are) missing"
        
        ##source
        #source code
        mv ./*.tar.gz ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) source code is(are) missing"
        #source rpms
        mv ./*.src.rpm ./$RELEASE_DIR 2>/dev/null ||\
            print_red "Warning: package(s) source code rpm is(are) missing"
        
        echo "All the generated packages can be found in the directory \"$RELEASE_DIR\""
    fi
}
############# end finish packaging #####

############# uploading to sourceforge.net #####
#we put the files on the sourceforge ftp
function upload_to_sourceforge()
{
    if [[ $ARCH = "i386" ]];then
        if [[ $UPLOAD_TO_SOURCEFORGE == 1 ]]; then
            echo
            print_yellow "Uploading files to sourceforge.net..."
            echo
            
            for a in ls $DIST_VERSION; do
                lftp -e "cd /incoming;put $DIST_VERSION/$a;quit" -u anonymous,\
                    upload.sourceforge.net || exit 1
            done
        fi
    fi
}
############# finish uploading to sourceforge.net #####

################################
############# main program #####
################################

DATE_START=`date`

#confirmation question
if [[ $1 != "quiet_root" ]];then
    confirmation_question
fi

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROJECT_DIR=$script_dir/..
cd $PROJECT_DIR

#we include the package variables
. ./${LIBMP3SPLT_DIR}/include_variables.sh "quiet_noflags"
. ./${MP3SPLT_DIR}/include_variables.sh "quiet_noflags"
. ./${MP3SPLT_GTK_DIR}/include_variables.sh "quiet_noflags"

#update the versions
update_versions

#i386 gnu/linux packages :
source_packages
debian_packages
ubuntu_packages
gnu_linux_static_packages
gnu_linux_dynamic_packages
gentoo_packages
rpm_packages
archlinux_packages
slackware_packages
#i386 windows cross installers :
windows_cross_installers
#amd64 gnu/linux packages :
amd64_packages
#i386 bsd-like packages :
openbsd_packages
netbsd_packages
freebsd_packages
#i386 gnu/opensolaris packages :
##nexenta#nexenta_packages() #slow
#finish packaging
finish_packaging
upload_to_sourceforge

#date info
if [[ $ARCH = "i386" ]];then
    echo
    print_green "The packaging is finished :)"

    DATE_END=`date`
    
    echo
    echo "Start_date : "$DATE_START
    echo "End_date : "$DATE_END
    echo
fi

############# cleaning the distribution #####
#if [[ $ARCH = "i386" ]];then
#    make -s distclean || exit 1
#fi
############# end cleaning the distribution #####

################################
######### end main program #####
################################

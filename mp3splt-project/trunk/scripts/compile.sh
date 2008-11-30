#!/bin/bash

################# variables to set ############

#program directories
LIBMP3SPLT_DIR=libmp3splt
MP3SPLT_DIR=newmp3splt
MP3SPLT_GTK_DIR=mp3splt-gtk

#program versions
LIBMP3SPLT_REAL_VERSION=0.5.3
MP3SPLT_REAL_VERSION=2.2.2
MP3SPLT_GTK_REAL_VERSION=0.5.3

unset SHELL

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
    print_red "This script is used by the developers to auto-create packages for releases."
    print_red "You should modify this script in order to use it on your computer."
    print_red "This script must be run on a x86_64 system (not i386)."
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
  echo
  print_yellow "Updating versions..."
  echo

  #we update the real versions
  #libmp3splt
  sed -i "s/LIBMP3SPLT_VERSION=.*/LIBMP3SPLT_VERSION=$LIBMP3SPLT_REAL_VERSION/" ./${LIBMP3SPLT_DIR}/include_variables.sh
  #mp3splt + its libmp3splt library
  sed -i "s/MP3SPLT_VERSION=.*/MP3SPLT_VERSION=$MP3SPLT_REAL_VERSION/" ./${MP3SPLT_DIR}/include_variables.sh
  sed -i "s/LIBMP3SPLT_VERSION=.*/LIBMP3SPLT_VERSION=$LIBMP3SPLT_REAL_VERSION/" ./${MP3SPLT_DIR}/include_variables.sh
  #mp3splt-gtk + its libmp3splt library
  sed -i "s/MP3SPLT_GTK_VERSION=.*/MP3SPLT_GTK_VERSION=$MP3SPLT_GTK_REAL_VERSION/" ./${MP3SPLT_GTK_DIR}/include_variables.sh
  sed -i "s/LIBMP3SPLT_VERSION=.*/LIBMP3SPLT_VERSION=$LIBMP3SPLT_REAL_VERSION/" ./${MP3SPLT_GTK_DIR}/include_variables.sh

  #we update the versions
  ./${LIBMP3SPLT_DIR}/update_version.sh || exit 1
  ./${MP3SPLT_DIR}/update_version.sh || exit 1
   ./${MP3SPLT_GTK_DIR}/update_version.sh || exit 1

  cd $PROJECT_DIR
}
################## end update versions ############

############# source packages ################
function source_packages()
{
  echo
  print_yellow "Creating source distribution..."

  make -s source_packages || exit 1
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
    sed -i "1,4s/libmp3splt (\(.*\)\.$1)/libmp3splt (\1)/" ${LIBMP3SPLT_DIR}/debian/changelog
    sed -i "1,4s/mp3splt (\(.*\)\.$1)/mp3splt (\1)/" ${MP3SPLT_DIR}/debian/changelog
    sed -i "1,4s/mp3splt-gtk (\(.*\)\.$1)/mp3splt-gtk (\1)/" ${MP3SPLT_GTK_DIR}/debian/changelog
}

#make the chroot debian flavors
#example : make_debian_flavor "ubuntu" "breezy" "i386"
#example : make_debian_flavor "debian" "sarge" "amd64"
function make_debian_flavor()
{
    echo
    print_yellow "Creating $3 $1 $2 packages..."
    echo
    
    #if we don't have the distribution files
    if [[ ! -f "mp3splt-gtk-${MP3SPLT_GTK_REAL_VERSION}.${2}_${3}.deb" ||
          ! -f "mp3splt-${MP3SPLT_REAL_VERSION}.${2}_${3}.deb" ||
          ! -f "libmp3splt-${LIBMP3SPLT_REAL_VERSION}.${2}_${3}.deb" ]];then
        
        put_debian_version "$2"
        dchroot -d -c $2_$3 "export LC_ALL=\"C\" && make -s debian_packages " || \
        {
          clean_debian_version "$2"
          exit 1
        }
        clean_debian_version "$2"
        cd $PROJECT_DIR
    fi
}

function debian_packages()
{
    DEBIAN_FLAVORS="lenny unstable"

    echo
    print_yellow "Creating debian packages..."

    for deb_version in $DEBIAN_FLAVORS;do
      make_debian_flavor "debian" $deb_version "i386"
      make_debian_flavor "debian" $deb_version "amd64"
    done

    cd $PROJECT_DIR
}
############# end debian packages ################

############# ubuntu packages ##########################
function ubuntu_packages()
{
    UBUNTU_FLAVORS="gutsy hardy intrepid"

    echo
    print_yellow "Creating ubuntu packages..."
    
    for deb_version in $UBUNTU_FLAVORS;do
      make_debian_flavor "ubuntu" $deb_version "i386"
      make_debian_flavor "ubuntu" $deb_version "amd64"
    done

    cd $PROJECT_DIR
}
############# end ubuntu packages ##########################

############# gnu/linux static build #####
function gnu_linux_static_packages()
{
    echo
    print_yellow "Creating gnu/linux static builds..."
    
    make -s static_packages || exit 1
    cd $PROJECT_DIR
}
############# end gnu/linux static build #####

############# gnu/linux dynamic build #####
function gnu_linux_dynamic_packages()
{
    echo
    print_yellow "Creating dynamic gnu/linux builds..."
    
    make -s dynamic_packages || exit 1
    cd $PROJECT_DIR
}
############# end gnu/linux dynamic build #####

############# gentoo ebuilds ################
function gentoo_packages()
{
  echo
  print_yellow "Creating gentoo ebuilds..."
  echo

  dchroot -d -c gentoo "make -s gentoo_ebuilds" || exit 1
  cd $PROJECT_DIR
}
############# end gentoo ebuilds ################

############# windows installers ################
function windows_cross_installers()
{
  echo
  print_yellow "Creating windows installers..."

  #if we don't have the distribution file
  DIST_FILE1="./mp3splt_${MP3SPLT_VERSION}_i386.exe"
  DIST_FILE2="./mp3splt-gtk_${MP3SPLT_GTK_VERSION}_i386.exe"
  if [[ ! -f $DIST_FILE1 || ! -f $DIST_FILE2 ]];then
    make -s windows_cross_installers || exit 1
  else
    echo
    print_cyan "We already have the $DIST_FILE1 distribution file"
    print_cyan "and the $DIST_FILE2 distribution file !"
  fi

  #suppose architecture is i386
  #if [[ -f mp3splt_${MP3SPLT_VERSION}_x86_64.exe ]];then
  #  mv mp3splt_${MP3SPLT_VERSION}_x86_64.exe $DIST_FILE1
  #fi
  #if [[ -f mp3splt-gtk_${MP3SPLT_GTK_VERSION}_x86_64.exe ]];then
  #  mv mp3splt-gtk_${MP3SPLT_GTK_VERSION}_x86_64.exe $DIST_FILE2
  #fi

  cd $PROJECT_DIR
}
############# end windows installers ################

############# RPM packages creation ################
function rpm_packages()
{
    echo
    print_yellow "Creating RPMs..."
    
    make -s rpm_packages || exit 1
    cd $PROJECT_DIR
}
############# end RPM packages creation ################

############# archlinux packages #########
function archlinux_packages()
{
    echo
    print_yellow "Creating archlinux packages..."
    echo
    
    dchroot -d -c arch "make -s arch_packages" || exit 1
    cd $PROJECT_DIR
}
############# end archlinux packages #########

############# slackware packages #########
function slackware_packages()
{
    echo
    print_yellow "Creating slackware packages..."
    echo
    
    dchroot -d -c slackware "make -s slackware_fakeroot_packages" || exit 1
    cd $PROJECT_DIR
}
############# end slackware packages #####

############# openbsd packages #####
function openbsd_packages()
{
  echo
  print_yellow "Creating openbsd packages..."

  #if we don't have the distribution file
  DIST_FILE1="libmp3splt_obsd_i386-${LIBMP3SPLT_VERSION}.tgz"
  DIST_FILE2="mp3splt_obsd_i386-${MP3SPLT_VERSION}.tgz"
  DIST_FILE3="mp3splt-gtk_obsd_i386-${MP3SPLT_GTK_VERSION}.tgz"
  #if the 3 files do not exist
  if ! [[ -f $DIST_FILE1 || -f $DIST_FILE2 || -f $DIST_FILE3 ]];then
    cd /mnt/personal/systems/bsd-based/openbsd && ./openbsd || exit 1
    cd $PROJECT_DIR
  else
    echo
    print_cyan "We already have the OpenBSD packages !"
  fi
}
############# end openbsd packages #####

############# netbsd packages #####
function netbsd_packages()
{
  echo
  print_yellow "Creating netbsd packages..."

  #if we don't have the distribution file
  DIST_FILE1="libmp3splt_nbsd_i386-${LIBMP3SPLT_VERSION}.tgz"
  DIST_FILE2="mp3splt_nbsd_i386-${MP3SPLT_VERSION}.tgz"
  DIST_FILE3="mp3splt-gtk_nbsd_i386-${MP3SPLT_GTK_VERSION}.tgz"
  if [[ ! -f $DIST_FILE1 || ! -f $DIST_FILE2 || ! -f $DIST_FILE3 ]];then
    cd /mnt/personal/systems/bsd-based/netbsd && ./netbsd || exit 1
    cd $PROJECT_DIR
  else
    echo
    print_cyan "We already have the NetBSD packages !"
  fi
}
############# end netbsd packages #####

############# freebsd packages #####
function freebsd_packages
{
  echo
  print_yellow "Creating freebsd packages..."

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
  DIST_FILE1="./libmp3splt_fbsd_i386-${NEW_LIBMP3SPLT_VERSION}.tbz"
  DIST_FILE2="./mp3splt_fbsd_i386-${NEW_MP3SPLT_VERSION}.tbz"
  DIST_FILE3="./mp3splt-gtk_fbsd_i386-${NEW_MP3SPLT_GTK_VERSION}.tbz"
  if [[ ! -f $DIST_FILE1 || ! -f $DIST_FILE2 || ! -f $DIST_FILE3 ]];then
    cd /mnt/personal/systems/bsd-based/freebsd && ./freebsd || exit 1
    cd $PROJECT_DIR
  else
    echo
    print_cyan "We already have the FreeBSD packages !"
  fi
}
############# end freebsd packages #####

############# nexenta gnu/opensolaris packages #####
function nexenta_packages()
{
  echo
  print_yellow "Creating nexenta gnu/opensolaris packages..."
  echo

  #if we don't have the distribution file
  DIST_FILE1="./libmp3splt_${LIBMP3SPLT_VERSION}_solaris-i386.deb"
  DIST_FILE2="./mp3splt_${MP3SPLT_VERSION}_solaris-i386.deb"
  DIST_FILE3="./mp3splt-gtk_${MP3SPLT_GTK_VERSION}_solaris-i386.deb"
  if [[ ! -f $DIST_FILE1 || ! -f $DIST_FILE2 || ! -f $DIST_FILE3 ]];then
    cd /mnt/personal/systems/opensolaris/ && ./nexenta || exit 1
    cd $PROJECT_DIR
  else
    echo
    print_cyan "We already have the Nexenta packages !"
  fi
}
############# end nexenta gnu/opensolaris packages #####

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

###################################
#update the versions
update_versions

###################################
#build packages
source_packages
debian_packages
ubuntu_packages
windows_cross_installers

#gnu_linux_static_packages
#gnu_linux_dynamic_packages
#gentoo_packages
#rpm_packages
#archlinux_packages
#slackware_packages
#amd64 gnu/linux packages :
#i386 bsd-like packages :
#openbsd_packages
#netbsd_packages
#freebsd_packages
#i386 gnu/opensolaris packages :
#nexenta_packages #slow

###################################
#finish packaging ...

#date info
echo
print_green "The packaging is finished :)"

DATE_END=`date`

echo
echo "Start_date : "$DATE_START
echo "End_date : "$DATE_END
echo

################################
######### end main program #####
################################


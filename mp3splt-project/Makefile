LIBMP3SPLT_DIR=libmp3splt
MP3SPLT_DIR=newmp3splt
MP3SPLT_GTK_DIR=mp3splt-gtk

usage: help

#help of this Makefile; explains how this makefile should be used
help:
	@echo
	@echo "The current directory must contain libmp3splt, newmp3splt and mp3splt-gtk."
	@echo
	@echo "Usage : "
	@echo
	@echo "make root_install : compile and installs the program (must be root)"
	@echo "make root_uninstall : uninstalls the programs as root (must be root)"
	@echo
	@echo "make source_packages : generate source packages"
	@echo
	@echo "make debian_packages : generate debian packages"
	@echo "make gentoo_ebuilds : generate gentoo ebuilds"
	@echo "make rpm_packages : generate rpm packages"
	@echo "make arch_packages : generate archlinux packages"
	@echo "make slackware_packages : generate slackware packages (must be root)"
	@echo "make slackware_fakeroot_packages : generate slackware packages by using fakeroot (should be root)"
	@echo "make static_packages : generates static packages"
	@echo "make dynamic_packages : generates dynamic packages"
	@echo
	@echo "make nexenta_packages : generates nexenta gnu/opensolaris packages"
	@echo
	@echo "make openbsd_packages : generates openbsd packages (must be root)"
	@echo "make netbsd_packages : generates netbsd packages (must be root)"
	@echo "make freebsd_packages : generates freebsd packages (must be root)"
	@echo
	@echo "make debian_hurd_packages : generates debian_hurd packages (must be root)"
	@echo 
	@echo "make windows_cross_installers : generates windows installers with a cross compilation"
	@echo

#source distribution creation
source_packages: clean_tmp
	./${LIBMP3SPLT_DIR}/make_source_package.sh
	./${MP3SPLT_DIR}/make_source_package.sh
	./${MP3SPLT_GTK_DIR}/make_source_package.sh

#debian packages creation
debian_packages: clean_tmp
	./${LIBMP3SPLT_DIR}/debian/make_debian_package.sh
	./${MP3SPLT_DIR}/debian/make_debian_package.sh
	./${MP3SPLT_GTK_DIR}/debian/make_debian_package.sh

#creates static packages
static_packages: clean_tmp
	./${LIBMP3SPLT_DIR}/make_static_package.sh
	./${MP3SPLT_DIR}/make_static_package.sh
	./${MP3SPLT_GTK_DIR}/make_static_package.sh

#creates dynamic packages
dynamic_packages: clean_tmp
	./${LIBMP3SPLT_DIR}/make_dynamic_package.sh
	./${MP3SPLT_DIR}/make_dynamic_package.sh
	./${MP3SPLT_GTK_DIR}/make_dynamic_package.sh

#gentoo ebuilds creation
gentoo_ebuilds: clean_tmp
	./${LIBMP3SPLT_DIR}/gentoo/make_gentoo_ebuild.sh
	./${MP3SPLT_DIR}/gentoo/make_gentoo_ebuild.sh
	./${MP3SPLT_GTK_DIR}/gentoo/make_gentoo_ebuild.sh

#rpm package creation
rpm_packages: clean_tmp
	./${LIBMP3SPLT_DIR}/rpm/make_rpm_package.sh
	./${MP3SPLT_DIR}/rpm/make_rpm_package.sh
	./${MP3SPLT_GTK_DIR}/rpm/make_rpm_package.sh

#arch linux package creation
arch_packages: clean_tmp
	./${LIBMP3SPLT_DIR}/arch/make_arch_package.sh
	./${MP3SPLT_DIR}/arch/make_arch_package.sh
	./${MP3SPLT_GTK_DIR}/arch/make_arch_package.sh
	cd ${LIBMP3SPLT_DIR}/arch && rm -rf ./pkg ./src ./filelist

#slackware package creation
slackware_packages: clean_tmp
	./${LIBMP3SPLT_DIR}/slackware/make_slackware_package.sh
	./${MP3SPLT_DIR}/slackware/make_slackware_package.sh
	./${MP3SPLT_GTK_DIR}/slackware/make_slackware_package.sh

#creates slackware packages by using fakeroot
#-does not work if /usr/lib /usr/include and /etc are not
#writable by the current user
slackware_fakeroot_packages: clean_tmp
	fakeroot ./${LIBMP3SPLT_DIR}/slackware/make_slackware_package.sh
	fakeroot ./${MP3SPLT_DIR}/slackware/make_slackware_package.sh
	fakeroot ./${MP3SPLT_GTK_DIR}/slackware/make_slackware_package.sh

#opensolaris nexenta packages
nexenta_packages: clean_tmp
	./${LIBMP3SPLT_DIR}/debian/make_nexenta_package.sh
	./${MP3SPLT_DIR}/debian/make_nexenta_package.sh
	./${MP3SPLT_GTK_DIR}/debian/make_nexenta_package.sh

#create windows installer by cross compilation
windows_cross_installers:
	./${LIBMP3SPLT_DIR}/windows/make_windows_cross_compilation.sh
	./${MP3SPLT_DIR}/windows/make_windows_cross_compilation.sh
	./${MP3SPLT_DIR}/windows/make_windows_cross_installer.sh
	./${MP3SPLT_GTK_DIR}/windows/make_windows_cross_compilation.sh
	./${MP3SPLT_GTK_DIR}/windows/make_windows_cross_installer.sh

#debian hurd packages creation
debian_hurd_packages: clean_tmp
	./${LIBMP3SPLT_DIR}/debian/make_debian_hurd_package.sh
	./${MP3SPLT_DIR}/debian/make_debian_hurd_package.sh
	./${MP3SPLT_GTK_DIR}/debian/make_debian_hurd_package.sh

#openbsd packages
openbsd_packages:
	./${LIBMP3SPLT_DIR}/openbsd/make_openbsd_package.sh
	./${MP3SPLT_DIR}/openbsd/make_openbsd_package.sh
	./${MP3SPLT_GTK_DIR}/openbsd/make_openbsd_package.sh
        #uninstall libmp3splt
	cd ./${LIBMP3SPLT_DIR} && make uninstall
	rm -rf /usr/local/share/doc/libmp3splt

#netbsd packages
netbsd_packages:
	./${LIBMP3SPLT_DIR}/netbsd/make_netbsd_package.sh
	./${MP3SPLT_DIR}/netbsd/make_netbsd_package.sh
	./${MP3SPLT_GTK_DIR}/netbsd/make_netbsd_package.sh
        #uninstall libmp3splt
	cd /usr/pkgsrc/audio/libmp3splt && bmake deinstall

#generate freebsd packages
freebsd_packages:
	./${LIBMP3SPLT_DIR}/freebsd/make_freebsd_package.sh
	./${MP3SPLT_DIR}/freebsd/make_freebsd_package.sh
	./${MP3SPLT_GTK_DIR}/freebsd/make_freebsd_package.sh
        #uninstall libmp3splt
	cd /usr/ports/audio/libmp3splt && make deinstall

#installs libmp3splt, mp3splt and mp3splt-gtk (must be run as root)
root_install: root_libmp3splt root_newmp3splt root_mp3splt-gtk
root_libmp3splt:
	cd ${LIBMP3SPLT_DIR} && ./autogen.sh && \
	./configure && make clean && make && make install
# \
#	&& if [ -z `grep '/usr/local/lib' /etc/ld.so.conf` ];\
#	then `echo '/usr/local/lib' >> /etc/ld.so.conf` && ldconfig;fi
root_newmp3splt:
	cd ${MP3SPLT_DIR} && ./autogen.sh && ./configure \
	&& make clean && make && make install
root_mp3splt-gtk:
	cd ${MP3SPLT_GTK_DIR} && ./autogen.sh && ./configure --enable-bmp\
	&& make clean && make && make install
#uninstalls libmp3splt, mp3splt and mp3splt-gtk as root
root_uninstall:
	cd ${LIBMP3SPLT_DIR} && make uninstall
	cd ${MP3SPLT_DIR} && make uninstall
	cd ${MP3SPLT_GTK_DIR} && make uninstall

#cleans the /tmp directories for a good build
DATE=`date +-%d_%m_%Y__%H_%M_%S`
clean_tmp:
	if [[ -e /tmp/dynamic_tmp ]];then \
mv /tmp/dynamic_tmp /tmp/dynamic_tmp${DATE} &>/dev/null;fi
	if [[ -e /tmp/static_tmp ]];then \
mv /tmp/static_tmp /tmp/static_tmp${DATE} &>/dev/null;fi
	if [[ -e /tmp/rpm_tmp ]];then \
mv /tmp/rpm_temp /tmp/rpm_temp${DATE} &>/dev/null;fi
	if [[ -e /tmp/slack_temp ]];then \
mv /tmp/slack_temp /tmp/slack_temp${DATE} &>/dev/null;fi
	if [[ -e /tmp/temp ]];then \
mv /tmp/temp /tmp/temp${DATE} &>/dev/null;fi
	if [[ -e /tmp/gentoo_temp ]];then \
mv /tmp/gentoo_temp /tmp/gentoo${DATE} &>/dev/null;fi
	sleep 1

#clean the builds
clean:
	cd ${LIBMP3SPLT_DIR} && make clean
	cd ${MP3SPLT_DIR} && make clean
	cd ${MP3SPLT_GTK_DIR} && make clean
distclean:
	cd ${LIBMP3SPLT_DIR} && make distclean
	cd ${MP3SPLT_DIR} && make distclean
	cd ${MP3SPLT_GTK_DIR} && make distclean

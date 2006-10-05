usage: help

#help of this Makefile; explains how this makefile should be used
help:
	@echo
	@echo "The current directory must contain libmp3splt, newmp3splt and mp3splt-gtk."
	@echo
	@echo "Usage : "
	@echo
	@echo "make root_install : compile and installs the programs (must be root)"
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
	./libmp3splt/make_source_package.sh
	./newmp3splt/make_source_package.sh
	./mp3splt-gtk/make_source_package.sh

#debian packages creation
debian_packages: clean_tmp
	./libmp3splt/debian/make_debian_package.sh
	./newmp3splt/debian/make_debian_package.sh
	./mp3splt-gtk/debian/make_debian_package.sh

#gentoo ebuilds creation
gentoo_ebuilds: clean_tmp
	./libmp3splt/gentoo/make_gentoo_ebuild.sh
	./newmp3splt/gentoo/make_gentoo_ebuild.sh
	./mp3splt-gtk/gentoo/make_gentoo_ebuild.sh

#rpm package creation
rpm_packages: clean_tmp
	./libmp3splt/rpm/make_rpm_package.sh
	./newmp3splt/rpm/make_rpm_package.sh
	./mp3splt-gtk/rpm/make_rpm_package.sh

#arch linux package creation
arch_packages: clean_tmp
	./libmp3splt/arch/make_arch_package.sh
	./newmp3splt/arch/make_arch_package.sh
	./mp3splt-gtk/arch/make_arch_package.sh
	cd libmp3splt/arch && rm -rf ./pkg ./src ./filelist

#slackware package creation
slackware_packages: clean_tmp
	./libmp3splt/slackware/make_slackware_package.sh
	./newmp3splt/slackware/make_slackware_package.sh
	./mp3splt-gtk/slackware/make_slackware_package.sh

#creates slackware packages by using fakeroot
#-does not work if /usr/lib /usr/include and /etc are not
#writable by the current user
slackware_fakeroot_packages: clean_tmp
	fakeroot ./libmp3splt/slackware/make_slackware_package.sh
	fakeroot ./newmp3splt/slackware/make_slackware_package.sh
	fakeroot ./mp3splt-gtk/slackware/make_slackware_package.sh

#creates static packages
static_packages: clean_tmp
	./libmp3splt/make_static_package.sh
	./newmp3splt/make_static_package.sh
	./mp3splt-gtk/make_static_package.sh

#creates dynamic packages
dynamic_packages: clean_tmp
	./libmp3splt/make_dynamic_package.sh
	./newmp3splt/make_dynamic_package.sh
	./mp3splt-gtk/make_dynamic_package.sh

#opensolaris nexenta packages
nexenta_packages: clean_tmp
	./libmp3splt/debian/make_nexenta_package.sh
	./newmp3splt/debian/make_nexenta_package.sh
	./mp3splt-gtk/debian/make_nexenta_package.sh

#create windows installer by cross compilation
windows_cross_installers:
	./libmp3splt/windows/make_windows_cross_compilation.sh
	./newmp3splt/windows/make_windows_cross_compilation.sh
	./newmp3splt/windows/make_windows_cross_installer.sh
	./mp3splt-gtk/windows/make_windows_cross_compilation.sh
	./mp3splt-gtk/windows/make_windows_cross_installer.sh

#debian hurd packages creation
debian_hurd_packages: clean_tmp
	./libmp3splt/debian/make_debian_hurd_package.sh
	./newmp3splt/debian/make_debian_hurd_package.sh
	./mp3splt-gtk/debian/make_debian_hurd_package.sh

#openbsd packages
openbsd_packages:
	./libmp3splt/openbsd/make_openbsd_package.sh
	./newmp3splt/openbsd/make_openbsd_package.sh
	./mp3splt-gtk/openbsd/make_openbsd_package.sh
        #uninstall libmp3splt
	cd ./libmp3splt && make uninstall
	rm -rf /usr/local/share/doc/libmp3splt

#netbsd packages
netbsd_packages:
	./libmp3splt/netbsd/make_netbsd_package.sh
	./newmp3splt/netbsd/make_netbsd_package.sh
	./mp3splt-gtk/netbsd/make_netbsd_package.sh
        #uninstall libmp3splt
	cd /usr/pkgsrc/audio/libmp3splt && bmake deinstall

#generate freebsd packages
freebsd_packages:
	./libmp3splt/freebsd/make_freebsd_package.sh
	./newmp3splt/freebsd/make_freebsd_package.sh
	./mp3splt-gtk/freebsd/make_freebsd_package.sh
        #uninstall libmp3splt
	cd /usr/ports/audio/libmp3splt && make deinstall && cd -

#installs libmp3splt, mp3splt and mp3splt-gtk (must be run as root)
root_install: root_libmp3splt root_newmp3splt root_mp3splt-gtk
root_libmp3splt:
	cd libmp3splt && ./autogen.sh && \
	./configure && make clean && make && make install \
	&& if [ -z `grep '/usr/local/lib' /etc/ld.so.conf` ];\
	then `echo '/usr/local/lib' >> /etc/ld.so.conf` && ldconfig;fi
root_newmp3splt:
	cd newmp3splt && ./autogen.sh && ./configure \
	&& make clean && make && make install
root_mp3splt-gtk:
	cd mp3splt-gtk && ./autogen.sh && ./configure \
	&& make clean && make && make install
#uninstalls libmp3splt, mp3splt and mp3splt-gtk as root
root_uninstall:
	cd libmp3splt && make uninstall
	cd newmp3splt && make uninstall
	cd mp3splt-gtk && make uninstall

#cleans the /tmp directories for a good build
DATE=`date +-%d_%m_%Y__%H_%M_%S`
clean_tmp:
	if [[ -e /tmp/dynamic_tmp ]];then \
mv /tmp/dynamic_tmp /tmp/dynamic_tmp${DATE}M &>/dev/null;fi
	if [[ -e /tmp/static_tmp ]];then \
mv /tmp/static_tmp /tmp/static_tmp${DATE}M &>/dev/null;fi
	if [[ -e /tmp/rpm_tmp ]];then \
mv /tmp/rpm_temp /tmp/rpm_temp${DATE}M &>/dev/null;fi
	if [[ -e /tmp/slack_temp ]];then \
mv /tmp/slack_temp /tmp/slack_temp${DATE}M &>/dev/null;fi
	if [[ -e /tmp/temp ]];then \
mv /tmp/temp /tmp/temp${DATE}M &>/dev/null;fi
	if [[ -e /tmp/gentoo_temp ]];then \
mv /tmp/gentoo_temp /tmp/gentoo${DATE}M &>/dev/null;fi

#clean the builds
clean: distclean
distclean:
	cd libmp3splt && make distclean && cd ..
	cd newmp3splt && make distclean && cd ..
	cd mp3splt-gtk && make distclean && cd ..

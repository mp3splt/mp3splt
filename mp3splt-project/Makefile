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
	@echo "make debian_packages : generate debian packages"
	@echo "make gentoo_ebuilds : generate gentoo ebuilds"
	@echo "make rpm_packages : generate rpm packages"
	@echo "make arch_packages : generate archlinux packages"
	@echo "make slackware_packages : generate slackware packages (must be root)"
	@echo "make slackware_fakeroot_packages : generate slackware packages by using fakeroot (should be root)"
	@echo "make static_packages : generates static packages"
	@echo "make dynamic_packages : generates dynamic packages"
	@echo "make nexenta_packages : generates nexenta gnu/opensolaris packages"
	@echo "make windows_cross_installers : generates windows installers with a cross compilation"
	@echo "make debian_hurd_packages : generates debian_hurd packages (must be root)"
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
clean_tmp:
	if [[ -d /tmp/dynamic_tmp ]];then rm -rf /tmp/dynamic_tmp_old2 &&\
mv /tmp/dynamic_tmp /tmp/dynamic_tmp_old2 &>/dev/null;fi
	if [[ -d /tmp/static_tmp ]];then rm -rf /tmp/static_tmp_old2 &&\
mv /tmp/static_tmp /tmp/static_tmp_old2 &>/dev/null;fi
	if [[ -d /tmp/rpm_tmp ]];then rm -rf /tmp/rpm_temp_old2 &&\
mv /tmp/rpm_temp /tmp/rpm_temp_old2 &>/dev/null;fi
	if [[ -d /tmp/slack_temp ]];then rm -rf /tmp/slack_temp_old2 &&\
mv /tmp/slack_temp /tmp/slack_temp_old2 &>/dev/null;fi
	if [[ -d /tmp/temp ]];then rm -rf /tmp/temp_old2 &&\
mv /tmp/temp /tmp/temp_old2 &>/dev/null;fi

#clean the builds
clean: distclean
distclean:
	cd libmp3splt && make distclean && cd ..
	cd newmp3splt && make distclean && cd ..
	cd mp3splt-gtk && make distclean && cd ..

#openbsd install
openbsd_root_install:
	export AUTOMAKE_VERSION=1.9 \
export AUTOCONF_VERSION=2.59 \
export CFLAGS="-I/usr/include -I/usr/local/include" \
export LDFLAGS="-L/usr/lib -L/usr/local/lib" \
&& cd libmp3splt && ./autogen.sh && ./configure \
&& make clean && make && make install &&\
cd ../newmp3splt && ./autogen.sh && ./configure \
&& make clean && make && make install &&\
cd ../mp3splt-gtk && ./autogen.sh && \
./configure --enable-bmp && make && make clean && make install

#openbsd uninstall
openbsd_root_uninstall:
	cd libmp3splt && make uninstall\
&& cd ../newmp3splt && make uninstall \
&& cd ../mp3splt-gtk && make uninstall

#open bsd packages
openbsd_packages:
	cd libmp3splt/openbsd && ./do_pkg.sh &&\
cd ../../newmp3splt/openbsd && ./do_pkg.sh &&\
cd ../../mp3splt-gtk/openbsd && ./do_pkg.sh &&\
cd ../../libmp3splt && make uninstall && \
rm -rf /usr/local/share/doc/libmp3splt

#netbsd packages
netbsd_packages:
	export CFLAGS="-I/usr/pkg/include" \
export LDFLAGS="-L/usr/pkg/lib";\
rm -f /root/progs_src/mp3splt/mp3splt-project/*mp3splt*nbsd*.tgz;\
rm -f /usr/pkgsrc/packages/All/*mp3splt*nbsd*.tgz;\
mkdir -p /usr/pkgsrc/audio/libmp3splt;\
mkdir -p /usr/pkgsrc/audio/mp3splt;\
mkdir -p /usr/pkgsrc/audio/mp3splt-gtk;\
rm -rf /usr/pkgsrc/audio/libmp3splt/*;\
rm -rf /usr/pkgsrc/audio/mp3splt/*;\
rm -rf /usr/pkgsrc/audio/mp3splt-gtk/*;\
cp -r /root/progs_src/mp3splt/mp3splt-project/libmp3splt/netbsd/* /usr/pkgsrc/audio/libmp3splt/;\
cp -r /root/progs_src/mp3splt/mp3splt-project/newmp3splt/netbsd/* /usr/pkgsrc/audio/mp3splt/;\
cp -r /root/progs_src/mp3splt/mp3splt-project/mp3splt-gtk/netbsd/* /usr/pkgsrc/audio/mp3splt-gtk/;\
cd /root/progs_src/mp3splt/mp3splt-project/libmp3splt;\
./autogen.sh && ./configure && make dist &&\
mv libmp3splt*.tar.gz /usr/pkgsrc/distfiles/;\
pkg_delete mp3splt-gtk-nbsd;\
pkg_delete mp3splt-nbsd;\
pkg_delete libmp3splt-nbsd;\
cd /usr/pkgsrc/audio/libmp3splt && bmake mdi && bmake package &&\
cd /root/progs_src/mp3splt/mp3splt-project/newmp3splt && ./autogen.sh && ./configure && make dist &&\
mv mp3splt*.tar.gz /usr/pkgsrc/distfiles/ &&\
cd ../mp3splt-gtk && ./autogen.sh && ./configure && make dist &&\
mv mp3splt-gtk*.tar.gz /usr/pkgsrc/distfiles/ &&\
cd /usr/pkgsrc/audio/mp3splt && bmake mdi && bmake package &&\
bmake deinstall && cd /usr/pkgsrc/audio/mp3splt-gtk && bmake mdi &&\
bmake package && bmake deinstall && cd /usr/pkgsrc/audio/libmp3splt &&\
bmake deinstall &&\
mv /usr/pkgsrc/packages/All/*mp3splt*nbsd*.tgz /root/progs_src/mp3splt/mp3splt-project

freebsd_packages:
	export ACLOCAL_FLAGS="-I /usr/local/share/aclocal" \
export CFLAGS="-I/usr/local/include -I/usr/include -I/usr/X11R6/include" \
export LDFLAGS="-L/usr/local/lib -L/usr/lib -L/usr/X11R6/lib";\
cd /root/progs_src/mp3splt-project/libmp3splt &&\
pkg_delete mp3splt-gtk-fbsd;\
pkg_delete mp3splt-fbsd;\
pkg_delete libmp3splt-fbsd;\
./autogen.sh && ./configure && make dist &&\
mv libmp3splt*.tar.gz /usr/ports/distfiles/ &&\
mkdir -p /usr/ports/audio/libmp3splt &&\
rm -rf /usr/ports/audio/libmp3splt/* &&\
cp -r /root/progs_src/mp3splt-project/libmp3splt/freebsd/* /usr/ports/audio/libmp3splt &&\
cd /usr/ports/audio/libmp3splt && make makesum && make && make install &&\
make package && mv *fbsd*.tbz /root/progs_src/mp3splt-project/ &&\
cd /root/progs_src/mp3splt-project/newmp3splt &&\
./autogen.sh && ./configure && make dist &&\
mv mp3splt*.tar.gz /usr/ports/distfiles &&\
mkdir -p /usr/ports/audio/mp3splt &&\
rm -rf /usr/ports/audio/mp3splt/* &&\
cp -r /root/progs_src/mp3splt-project/newmp3splt/freebsd/* /usr/ports/audio/mp3splt &&\
cd /usr/ports/audio/mp3splt && make makesum && make && make install && make package &&\
mv *fbsd*.tbz /root/progs_src/mp3splt-project/ && make deinstall &&\
cd /root/progs_src/mp3splt-project/mp3splt-gtk &&\
./autogen.sh && ./configure && make dist &&\
mv mp3splt-gtk*.tar.gz /usr/ports/distfiles &&\
mkdir -p /usr/ports/audio/mp3splt-gtk &&\
rm -rf /usr/ports/audio/mp3splt-gtk/* &&\
cp -r /root/progs_src/mp3splt-project/mp3splt-gtk/freebsd/* /usr/ports/audio/mp3splt-gtk &&\
cd /usr/ports/audio/mp3splt-gtk && make makesum && make && make install && make package &&\
mv *fbsd*.tbz /root/progs_src/mp3splt-project/ && make deinstall &&\
cd /usr/ports/audio/libmp3splt && make deinstall

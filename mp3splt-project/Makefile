all: 
	@echo "Usage : ..."

#source distribution creation
source_packages:
	./libmp3splt/make_source_package.sh
	./newmp3splt/make_source_package.sh
	./mp3splt-gtk/make_source_package.sh

#debian packages creation
debian_packages:
	./libmp3splt/debian/make_debian_package.sh
	./newmp3splt/debian/make_debian_package.sh
	./mp3splt-gtk/debian/make_debian_package.sh

#gentoo ebuilds creation
gentoo_ebuilds:
	./libmp3splt/gentoo/make_gentoo_ebuild.sh
	./newmp3splt/gentoo/make_gentoo_ebuild.sh
	./mp3splt-gtk/gentoo/make_gentoo_ebuild.sh

#creates static packages
static_packages:
	./libmp3splt/make_static_package.sh
	./newmp3splt/make_static_package.sh
	./mp3splt-gtk/make_static_package.sh

#creates dynamic packages
dynamic_packages:
	./libmp3splt/make_dynamic_package.sh
	./newmp3splt/make_dynamic_package.sh
	./mp3splt-gtk/make_dynamic_package.sh

#debian hurd packages creation
debian_hurd_packages:
	./libmp3splt/debian/make_debian_hurd_package.sh
	./newmp3splt/debian/make_debian_hurd_package.sh
	./mp3splt-gtk/debian/make_debian_hurd_package.sh

#clean the builds
clean: distclean
distclean:
	cd libmp3splt && make distclean && cd ..
	cd newmp3splt && make distclean && cd ..
	cd mp3splt-gtk && make distclean && cd ..

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

#opensolaris nexenta packages
nexenta_packages: libmp3splt_solaris mp3splt-gtk_solaris mp3splt_solaris
	rm -rf /tmp/temp

#we make libmp3splt packages
libmp3splt_solaris:
	cd libmp3splt && ./autogen.sh && \
	./configure --prefix=/tmp/temp --host=i386-pc-solaris2.11 && make clean && \
	make && make install && \
	./configure --prefix=/usr && make clean && make \
	&& debian/rules binary && cd ..

#we make mp3splt-gtk packages
mp3splt-gtk_solaris:
	export CFLAGS="${CFLAGS} -I/tmp/temp/include" \
	export LDFLAGS="${LDFLAGS} -L/tmp/temp/lib" &&\
	cd mp3splt-gtk && ./autogen.sh && \
	./configure --enable-bmp --host=i386-pc-solaris2.11 --prefix=/usr && make clean && \
	make && \
	export CFLAGS="${CFLAGS} -I/tmp/temp/include" \
	export LDFLAGS="${LDFLAGS} -L/tmp/temp/lib" && \
	debian/rules binary && cd ..

#we make mp3splt packages
newmp3splt_solaris:
	export CFLAGS="${CFLAGS} -I/tmp/temp/include" \
	export LDFLAGS="${LDFLAGS} -L/tmp/temp/lib" &&\
	cd newmp3splt && ./autogen.sh && \
	./configure --host=i386-pc-solaris2.11 --prefix=/usr && make clean && \
	make && \
	export CFLAGS="${CFLAGS} -I/tmp/temp/include" \
	export LDFLAGS="${LDFLAGS} -L/tmp/temp/lib" &&\
	debian/rules binary && cd ..

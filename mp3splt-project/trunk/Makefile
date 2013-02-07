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

#create windows installer by cross compilation
windows_cross_installers:
	./${LIBMP3SPLT_DIR}/windows/make_windows_cross_compilation.sh
	./${MP3SPLT_DIR}/windows/make_windows_cross_compilation.sh
	./${MP3SPLT_DIR}/windows/make_windows_installer.sh cross
	./${MP3SPLT_GTK_DIR}/windows/make_windows_cross_compilation.sh
	./${MP3SPLT_GTK_DIR}/windows/make_windows_installer.sh cross
	./${MP3SPLT_DIR}/windows/make_windows_zip.sh cross
	./${MP3SPLT_GTK_DIR}/windows/make_windows_zip.sh cross
	rm -rf translations

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
	cd ${MP3SPLT_GTK_DIR} && ./autogen.sh && ./configure --enable-audacious --enable-gstreamer \
	&& make clean && make && make install
#uninstalls libmp3splt, mp3splt and mp3splt-gtk as root
root_uninstall:
	cd ${LIBMP3SPLT_DIR} && make uninstall
	cd ${MP3SPLT_DIR} && make uninstall
	cd ${MP3SPLT_GTK_DIR} && make uninstall

#cleans the /tmp directories for a good build
DATE=`date +-%d_%m_%Y__%H_%M_%S`
clean_tmp:
	$(shell if [[ -e /tmp/temp ]];then \
mv /tmp/temp /tmp/temp${DATE} &>/dev/null;fi)
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

vim:
	screen vim libmp3splt/src/*.{c,h} libmp3splt/plugins/*.{c,h} libmp3splt/include/libmp3splt/*.h newmp3splt/src/*.c mp3splt-gtk/src/*.{c,h}

compile_all:
	cd ${LIBMP3SPLT_DIR} && ./autogen.sh && ./configure && make clean && make
	cd ${MP3SPLT_DIR} && ./autogen.sh && ./configure && make clean && make
	cd ${MP3SPLT_GTK_DIR} && ./autogen.sh && ./configure && make clean && make

compile:
	cd ${LIBMP3SPLT_DIR} && make
	cd ${MP3SPLT_DIR} && make
	cd ${MP3SPLT_GTK_DIR} && make


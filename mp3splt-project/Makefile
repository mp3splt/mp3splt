all: libmp3splt mp3splt-gtk newmp3splt

clean: clean_libmp3splt clean_mp3splt-gtk clean_newmp3splt

#the ebuilds are made in the ./scripts/compile.sh script

#we make libmp3splt packages
libmp3splt: clean_libmp3splt libmp3splt_dist libmp3splt_debian
clean_libmp3splt:
	cd libmp3splt;if [ -e Makefile ];then make clean;fi;cd ..
libmp3splt_dist:
	cd libmp3splt && ./autogen.sh && \
	./configure --prefix=/tmp/temp && make clean && \
	make && make install && \
	./configure --prefix=/usr && make clean && make && \
	make dist
libmp3splt_debian:
	cd libmp3splt && debian/rules binary && cd ..

#we make mp3splt-gtk packages
mp3splt-gtk: clean_mp3splt-gtk mp3splt-gtk_dist mp3splt-gtk_debian
clean_mp3splt-gtk:
	cd mp3splt-gtk;if [ -e Makefile ];then make clean;fi;cd ..
mp3splt-gtk_dist:
	cd mp3splt-gtk && ./autogen.sh && \
	./configure --enable-bmp --prefix=/usr && make clean && \
	make && make dist
mp3splt-gtk_debian:
	cd mp3splt-gtk && debian/rules binary && cd ..

#we make mp3splt packages
newmp3splt: clean_newmp3splt newmp3splt_dist newmp3splt_debian
clean_newmp3splt:
	cd newmp3splt;if [ -e Makefile ];then make clean;fi;cd ..
newmp3splt_dist:
	cd newmp3splt && ./autogen.sh && \
	./configure --prefix=/usr && make clean && \
	make && make dist
newmp3splt_debian:
	cd newmp3splt && debian/rules binary && cd ..

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
root_uninstall: root_unlibmp3splt root_unnewmp3splt root_unmp3splt-gtk
root_unlibmp3splt:
	cd libmp3splt && make uninstall
root_unnewmp3splt:
	cd newmp3splt && make uninstall
root_unmp3splt-gtk:
	cd mp3splt-gtk && make uninstall

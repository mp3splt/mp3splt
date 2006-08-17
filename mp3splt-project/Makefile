all: libmp3splt mp3splt-gtk newmp3splt

clean: clean_libmp3splt clean_mp3splt-gtk clean_newmp3splt

libmp3splt: clean_libmp3splt
	cd libmp3splt && ./autogen.sh && \
	./configure --prefix=`pwd`/../temp && make clean && \
	make && make install && \
	./configure --prefix=/usr && make clean && make && \
	make dist && debian/rules binary && cd ..

clean_libmp3splt:
	cd libmp3splt && make clean && cd ..

mp3splt-gtk: clean_mp3splt-gtk
	cd mp3splt-gtk && ./autogen.sh && \
	./configure --enable-bmp --prefix=/usr && make clean && \
	make && make dist && debian/rules binary && cd ..

clean_mp3splt-gtk:
	cd mp3splt-gtk && make clean && cd ..

newmp3splt: clean_newmp3splt
	cd newmp3splt && ./autogen.sh && \
	./configure --prefix=/usr && make clean && \
	make && make dist && cd ..

clean_newmp3splt:
	cd newmp3splt && make clean && cd ..

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

root_uninstall: root_unlibmp3splt root_unnewmp3splt root_unmp3splt-gtk

root_unlibmp3splt:
	cd libmp3splt && make uninstall

root_unnewmp3splt:
	cd newmp3splt && make uninstall

root_unmp3splt-gtk:
	cd mp3splt-gtk && make uninstall

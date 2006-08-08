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

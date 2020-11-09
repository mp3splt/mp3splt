## Basic instructions to compile on a *nix-like system

### libmp3splt compilation

Install build & documentation dependencies: `autopoint`, `autoconf`, `libtool`, `gettext`, `doxygen`, `graphviz`.
And needed libraries: `libmad`, `libogg`, `libvorbis`, `libid3tag`, `libflac`.
Example on a Debian-like system:
```bash
sudo apt-get install autopoint autoconf libtool gettext doxygen graphviz libmad0-dev libogg-dev libvorbis-dev libid3tag0-dev libflac-dev
```

To compile `libmp3splt`, go inside the `libmp3splt` directory and type the following commands:
```bash
mkdir target
./autogen.sh
./configure --prefix=`pwd`/target
make
make install
```

### mp3splt compilation

Once `libmp3splt` is compiled, go inside the `newmp3splt` directory and type the following commands:
```bash
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:`pwd`/../libmp3splt/target/lib/pkgconfig/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/../libmp3splt/target/lib/ # needed for running the final executable
./autogen.sh
./configure
make
```

`mp3splt` executable can be found in `newmp3splt/src/mp3splt`.

### mp3splt-gtk compilation

Install needed development files for `audacious`, `gstreamer`, `gtk-3`, `libaudclient` & `libdbus-glib`
Example on a Debian-like system:
```bash
sudo apt-get install audacious-dev libgstreamer1.0-dev libgtk-3-dev libaudclient-dev libdbus-glib-1-dev
```

Make sure to compile `libmp3splt` before this step.
Once `libmp3splt` has been compiled, go inside the `mp3splt-gtk` directory and type the following commands:
```bash
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:`pwd`/../libmp3splt/target/lib/pkgconfig/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/../libmp3splt/target/lib/ # needed for running the final executable
./autogen.sh
./configure
make
```

`mp3splt-gtk` executable can be found in `mp3splt-gtk/src/mp3splt-gtk`.

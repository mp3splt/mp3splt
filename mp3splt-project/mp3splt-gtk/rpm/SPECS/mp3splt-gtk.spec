Summary: Mp3splt-gtk is a GTK2 gui that uses libmp3splt to split mp3 and ogg without deconding
Name: mp3splt-gtk
Version: 0.4_rc1
Release: 1
Source: %{name}-%{version}.tar.gz
License: GNU GPL
Packager: Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>
Group: Productivity/Multimedia/Sound/Utilities
URL: http://mp3splt.sourceforge.net
BuildRoot: /tmp/rpm_temp/mp3splt-gtk
Requires: libmp3splt = 0.4_rc1, beep-media-player
%description
mp3splt-project common features:
* split mp3 and ogg files from a begin time to an end time without decoding
* split an album with splitpoints from the freedb.org server
* split an album with local .XMCD, .CDDB or .CUE file
* split files automatically with silence detection
* split files by a fixed time length
* split files created with Mp3Wrap or AlbumWrap
* split concatenated mp3 files
* support for mp3 VBR (variable bit rate)
* specify output directory for splitted files
%prep
%setup
%build
./configure --prefix=/usr --enable-bmp
make
%install
make DESTDIR=$RPM_BUILD_ROOT install
%clean
rm -rf $RPM_BUILD_ROOT/*
%files
%defattr(-,root,root)
/usr/*
%doc AUTHORS ChangeLog COPYING INSTALL NEWS README TODO

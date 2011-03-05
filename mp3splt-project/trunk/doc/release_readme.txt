Release instructions:
---------------------

0) set in 'tests/constants_variables.sh' RUN_INTERNET_TESTS to 1
1) update translation files:
    $ ./scripts/update_translation_files.sh 
2) check documentation of 'mp3splt -h'
3) check documentation in 'newmp3splt/doc/mp3splt.1'
4) convert documentation to html:
    $ ./newmp3splt/doc/generate_manual.sh
5) translate (string freeze)
    (with 'poedit' for example)
6) update the following files if necessary:
     ChangeLog
     NEWS
     AUTHORS
     INSTALL
7) change the following file and comment the debug flags:
     newmp3splt/configure.ac
     libmp3splt/configure.ac
     mp3splt-gtk/configure.ac
8) edit 'scripts/compile.sh' file and modify the following variables as needed:
     LIBMP3SPLT_REAL_VERSION=0.6.8
     MP3SPLT_REAL_VERSION=2.2.7
     MP3SPLT_GTK_REAL_VERSION=0.5.8
     BUILD_BINARY_PACKAGES=0
9) optional: update debian/ubuntu chroot installation for binary packages
10) create the distribution files:
     $ ./scripts/compile.sh
11) upload to sourceforge
12) change web site & upload web pages to sourceforge
13) update debian repository
14) make release announcement on sourceforge
15) make release announcement on mailing list


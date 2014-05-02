Release instructions:
---------------------

-1) verify
    libmp3splt_la_LDFLAGS = -no-undefined -version-info 0:9:0

0) set in 'tests/constants_variables.sh' RUN_INTERNET_TESTS to 1
1) update translation files:
    $ ./scripts/update_translation_files.sh 
2) check documentation of 'mp3splt -h'
3) check documentation in 'newmp3splt/doc/mp3splt.1'
4) translate (string freeze) (with 'poedit' for example)
5) update the following files if necessary:
     ChangeLog
     NEWS
     AUTHORS
     INSTALL
6) edit 'scripts/compile.sh' file and modify the following variables as needed:
     LIBMP3SPLT_REAL_VERSION=0.9.1
     MP3SPLT_REAL_VERSION=2.6.1
     MP3SPLT_GTK_REAL_VERSION=0.9.1
     BUILD_BINARY_PACKAGES=0
7) optional: update debian/ubuntu chroot installation for binary packages
8) create the distribution files:
     $ ./scripts/compile.sh
9) upload to sourceforge
10) generate libmp3splt API documentation:
    $ make -C libmp3splt/doc api_doc
11) convert documentation to html:
    $ ./newmp3splt/doc/generate_manual.sh
12) change web site & upload web pages to sourceforge
13) update debian repository
14) make release announcement on sourceforge
15) make release announcement on mailing list
16) create release branch


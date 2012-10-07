Release instructions:
---------------------

0) set in 'tests/constants_variables.sh' RUN_INTERNET_TESTS to 1
1) update translation files:
    $ ./scripts/update_translation_files.sh 
2) check documentation of 'mp3splt -h'
3) check documentation in 'newmp3splt/doc/mp3splt.1'
4) convert documentation to html:
    $ ./newmp3splt/doc/generate_manual.sh
4') generate libmp3splt API documentation:
    $ make -C libmp3splt/doc api_doc
5) translate (string freeze)
    (with 'poedit' for example)
5') update transifex translations 
6) update the following files if necessary:
     ChangeLog
     NEWS
     AUTHORS
     INSTALL
7) edit 'scripts/compile.sh' file and modify the following variables as needed:
     LIBMP3SPLT_REAL_VERSION=0.7
     MP3SPLT_REAL_VERSION=2.4
     MP3SPLT_GTK_REAL_VERSION=0.7
     BUILD_BINARY_PACKAGES=0
8) optional: update debian/ubuntu chroot installation for binary packages
9) create the distribution files:
     $ ./scripts/compile.sh
10) upload to sourceforge
11) change web site & upload web pages to sourceforge
12) update debian repository
13) make release announcement on sourceforge
14) make release announcement on mailing list
15) create branch ?


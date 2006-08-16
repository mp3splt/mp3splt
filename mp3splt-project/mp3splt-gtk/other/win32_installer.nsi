!define VERSION "0.3.1"
!define MP3SPLT_PATH c:/mp3splt_mingw/home/admin/mp3splt-project/mp3splt-gtk/other/../..

;name of the program
Name "mp3splt-gtk ${VERSION}"
;file to write
OutFile "mp3splt-gtk_${VERSION}.exe"
;installation directory
InstallDir $PROGRAMFILES\mp3splt-gtk

;Pages
Page license
Page components
Page directory
Page instfiles
LicenseData ${MP3SPLT_PATH}\mp3splt-gtk\COPYING
UninstPage uninstConfirm
UninstPage instfiles

;installer
Section ""
  SetOutPath $INSTDIR
  CreateDirectory "$INSTDIR\mp3splt-gtk_doc"
  CreateDirectory "$INSTDIR\libmp3splt_doc"
  CreateDirectory "$INSTDIR\locale\fr\LC_MESSAGES"

;locale files (languages)
;  SetOutPath $INSTDIR\locale\fr\LC_MESSAGES
;  File ${MP3SPLT_PATH}\fr_locales\mp3splt-gtk.mo
 
  SetOutPath $INSTDIR\mp3splt-gtk_doc
  File ${MP3SPLT_PATH}\mp3splt-gtk\README
  File ${MP3SPLT_PATH}\mp3splt-gtk\COPYING
  File ${MP3SPLT_PATH}\mp3splt-gtk\ChangeLog
  File ${MP3SPLT_PATH}\mp3splt-gtk\INSTALL
  File ${MP3SPLT_PATH}\mp3splt-gtk\NEWS
  File ${MP3SPLT_PATH}\mp3splt-gtk\TODO
  File ${MP3SPLT_PATH}\mp3splt-gtk\AUTHORS

  SetOutPath $INSTDIR\libmp3splt_doc
  File ${MP3SPLT_PATH}\libmp3splt\README
  File ${MP3SPLT_PATH}\libmp3splt\COPYING
  File ${MP3SPLT_PATH}\libmp3splt\ChangeLog
  File ${MP3SPLT_PATH}\libmp3splt\INSTALL
  File ${MP3SPLT_PATH}\libmp3splt\NEWS
  File ${MP3SPLT_PATH}\libmp3splt\TODO
  File ${MP3SPLT_PATH}\libmp3splt\AUTHORS

;copy gtk runtime
  SetOutPath $INSTDIR
  File /r ${MP3SPLT_PATH}\mp3splt-gtk_runtime\*.*

;copy the main executable
  File ${MP3SPLT_PATH}\mp3splt-gtk\src\mp3splt-gtk.exe
;copying the icons
  File ${MP3SPLT_PATH}\mp3splt-gtk\src\mp3splt-gtk.png
  File ${MP3SPLT_PATH}\mp3splt-gtk\src\mp3splt-gtk_ico.png
  File ${MP3SPLT_PATH}\mp3splt-gtk\other\mp3splt.ico
  
  WriteUninstaller "mp3splt-gtk_uninst.exe"
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\mp3splt-gtk"
  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\Uninstall.lnk" "$INSTDIR\mp3splt-gtk_uninst.exe" "" "$INSTDIR\mp3splt-gtk_uninst.exe" 0
  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\mp3splt-gtk.lnk" "$INSTDIR\mp3splt-gtk.exe" "" "$INSTDIR\mp3splt.ico"
  CreateShortCut "$DESKTOP\mp3splt-gtk.lnk" "$INSTDIR\mp3splt-gtk.exe" "" "$INSTDIR\mp3splt.ico"
  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\mp3splt-gtk_doc.lnk" "$INSTDIR\mp3splt-gtk_doc" "" "$INSTDIR\mp3splt-gtk_doc"
  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\libmp3splt_doc.lnk" "$INSTDIR\libmp3splt_doc" "" "$INSTDIR\libmp3splt_doc"
SectionEnd

;uninstaller
Section "Uninstall"
;delete gtk runtimes

;delete mp3splt-gtk files and directories
  RMDir /r "$INSTDIR"

;delete shortcuts  
  Delete "$SMPROGRAMS\mp3splt-gtk\*.*"
  Delete "$DESKTOP\Mp3Splt-Gtk.lnk"
  RMDir "$SMPROGRAMS\mp3splt-gtk"
SectionEnd


!define VERSION "0.5"
!define PROGRAM_NAME "mp3splt-gtk"
!define MP3SPLT_PATH c:/mp3splt_mingw/home/alex/mp3splt-project/mp3splt-gtk/windows/installer/../../..

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

;uninstall the old program if necessary
Function .onInit
  ReadRegStr $R0 HKLM "Software\${PROGRAM_NAME}\" "UninstallString"
  StrCmp $R0 "" done
 
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
  "${PROGRAM_NAME} is already installed. $\n$\nClick `OK` to remove the \
  previous version or `Cancel` to cancel this upgrade." \
  IDOK uninst
  Abort
;Run the uninstaller
uninst:
  ClearErrors
  ExecWait '$R0 _?=$INSTDIR'
  IfErrors no_remove_uninstaller
  no_remove_uninstaller:
done:
FunctionEnd

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
;copy the library
  File ${MP3SPLT_PATH}\libmp3splt\src\.libs\libmp3splt.dll
;copy the dependencies
  File ${MP3SPLT_PATH}\libltdl3.dll
  File ${MP3SPLT_PATH}\libogg-0.dll
  File ${MP3SPLT_PATH}\libvorbis-0.dll
  File ${MP3SPLT_PATH}\libvorbisenc-2.dll
  File ${MP3SPLT_PATH}\libvorbisfile-3.dll
  File ${MP3SPLT_PATH}\libmad-0.dll
  File ${MP3SPLT_PATH}\libid3tag.dll
;copy the plugins
  File ${MP3SPLT_PATH}\libmp3splt\plugins\.libs\libsplt_mp3.dll
  File ${MP3SPLT_PATH}\libmp3splt\plugins\.libs\libsplt_ogg.dll
;copying the icons
  File ${MP3SPLT_PATH}\mp3splt-gtk\src\mp3splt-gtk.png
  File ${MP3SPLT_PATH}\mp3splt-gtk\src\mp3splt-gtk_ico.png
  File ${MP3SPLT_PATH}\mp3splt-gtk\windows\installer\mp3splt.ico
  
  WriteUninstaller "mp3splt-gtk_uninst.exe"

  WriteRegStr HKLM "Software\${PROGRAM_NAME}\" "UninstallString" \
	"$INSTDIR\${PROGRAM_NAME}_uninst.exe"
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\mp3splt-gtk"

  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\libmp3splt_doc.lnk" "$INSTDIR\libmp3splt_doc" "" "$INSTDIR\libmp3splt_doc"
  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\mp3splt-gtk.lnk" "$INSTDIR\mp3splt-gtk.exe" "" "$INSTDIR\mp3splt.ico"
	CreateShortCut "$SMPROGRAMS\mp3splt-gtk\uninstall.lnk" "$INSTDIR\mp3splt-gtk_uninst.exe" "" "$INSTDIR\mp3splt-gtk_uninst.exe" 0
  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\mp3splt-gtk_doc.lnk" "$INSTDIR\mp3splt-gtk_doc" "" "$INSTDIR\mp3splt-gtk_doc"

  CreateShortCut "$DESKTOP\mp3splt-gtk.lnk" "$INSTDIR\mp3splt-gtk.exe" "" "$INSTDIR\mp3splt.ico"
SectionEnd

;uninstaller
Section "Uninstall"
;delete registry
  DeleteRegKey HKLM "Software\${PROGRAM_NAME}"

;delete mp3splt-gtk files and directories
  RMDir /r "$INSTDIR"

;delete shortcuts  
  Delete "$SMPROGRAMS\mp3splt-gtk\*.*"
  Delete "$DESKTOP\Mp3Splt-Gtk.lnk"
  RMDir "$SMPROGRAMS\mp3splt-gtk"
SectionEnd


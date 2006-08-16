!define VERSION "2.2.1"
!define MP3SPLT_PATH c:/mp3splt_mingw/home/admin/mp3splt-project/newmp3splt/other/../..

;name of the program
Name "mp3splt ${VERSION}"
;file to write
OutFile "mp3splt_${VERSION}.exe"
;installation directory
InstallDir $PROGRAMFILES\mp3splt

;Pages
Page license
Page components
Page directory
Page instfiles
LicenseData ${MP3SPLT_PATH}\newmp3splt\COPYING
UninstPage uninstConfirm
UninstPage instfiles

;installer
Section ""
  SetOutPath $INSTDIR
;copy the main executable
  File ${MP3SPLT_PATH}\newmp3splt\src\mp3splt.exe

  FileOpen $9 mp3splt.bat w
  FileWrite $9 'cmd /K "cd $INSTDIR & .\mp3splt.exe"'
  FileClose $9

;documentation
  CreateDirectory "$INSTDIR\mp3splt_doc"
  SetOutPath $INSTDIR\mp3splt_doc
  File ${MP3SPLT_PATH}\newmp3splt\README
  File ${MP3SPLT_PATH}\newmp3splt\COPYING
  File ${MP3SPLT_PATH}\newmp3splt\ChangeLog
  File ${MP3SPLT_PATH}\newmp3splt\INSTALL
  File ${MP3SPLT_PATH}\newmp3splt\NEWS
  File ${MP3SPLT_PATH}\newmp3splt\TODO
  File ${MP3SPLT_PATH}\newmp3splt\AUTHORS

  WriteUninstaller "mp3splt_uninst.exe"
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\mp3splt"
  CreateShortCut "$DESKTOP\Mp3Splt.lnk" "$INSTDIR\mp3splt.bat" "" ""
  CreateShortCut "$SMPROGRAMS\mp3splt\Uninstall.lnk" "$INSTDIR\mp3splt_uninst.exe" "" "$INSTDIR\mp3splt_uninst.exe" 0
  CreateShortCut "$SMPROGRAMS\mp3splt\mp3splt.lnk" "$INSTDIR\mp3splt.bat" "" ""
  CreateShortCut "$SMPROGRAMS\mp3splt\mp3splt_doc.lnk" "$INSTDIR\mp3splt_doc" "" "$INSTDIR\mp3splt_doc"
SectionEnd

;uninstaller
Section "Uninstall"

;delete mp3splt files and directories
  RMDir /r "$INSTDIR"

;delete shortcuts  
  Delete "$SMPROGRAMS\mp3splt\*.*"
  Delete "$DESKTOP\Mp3Splt.lnk"
  RMDir "$SMPROGRAMS\mp3splt"
SectionEnd

!define VERSION "2.2_rc1"
!define PROGRAM_NAME "mp3splt"
!define MP3SPLT_PATH c:/mp3splt_mingw/home/admin/mp3splt-project/newmp3splt/windows/installer/../../..

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
;  File ${MP3SPLT_PATH}\newmp3splt\NEWS
  File ${MP3SPLT_PATH}\newmp3splt\TODO
  File ${MP3SPLT_PATH}\newmp3splt\AUTHORS

  CreateDirectory "$INSTDIR\libmp3splt_doc"
  SetOutPath $INSTDIR\libmp3splt_doc
  File ${MP3SPLT_PATH}\libmp3splt\README
  File ${MP3SPLT_PATH}\libmp3splt\COPYING
  File ${MP3SPLT_PATH}\libmp3splt\ChangeLog
  File ${MP3SPLT_PATH}\libmp3splt\INSTALL
  File ${MP3SPLT_PATH}\libmp3splt\NEWS
  File ${MP3SPLT_PATH}\libmp3splt\TODO
  File ${MP3SPLT_PATH}\libmp3splt\AUTHORS

  WriteUninstaller "${PROGRAM_NAME}_uninst.exe"

  WriteRegStr HKLM "Software\${PROGRAM_NAME}\" "UninstallString" \
	"$INSTDIR\${PROGRAM_NAME}_uninst.exe"
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\mp3splt"
 
  CreateShortCut "$SMPROGRAMS\mp3splt\Mp3splt.lnk" "$INSTDIR\mp3splt.bat" "" ""
	
	CreateShortCut "$SMPROGRAMS\mp3splt\uninstall.lnk" "$INSTDIR\mp3splt_uninst.exe" "" "$INSTDIR\mp3splt_uninst.exe" 0
  CreateShortCut "$SMPROGRAMS\mp3splt\libmp3splt_doc.lnk" "$INSTDIR\libmp3splt_doc"	"" "$INSTDIR\libmp3splt_doc" 
  CreateShortCut "$SMPROGRAMS\mp3splt\mp3splt_doc.lnk" "$INSTDIR\mp3splt_doc" "" "$INSTDIR\mp3splt_doc" 
  
	CreateShortCut "$DESKTOP\mp3splt.lnk" "$INSTDIR\mp3splt.bat" "" ""  
SectionEnd

;uninstaller
Section "Uninstall"
;delete registry
  DeleteRegKey HKLM "Software\${PROGRAM_NAME}"

;delete mp3splt files and directories
  RMDir /r "$INSTDIR"

;delete shortcuts  
  Delete "$SMPROGRAMS\mp3splt\*.*"
  Delete "$DESKTOP\mp3splt.lnk"
  RMDir "$SMPROGRAMS\mp3splt"
SectionEnd

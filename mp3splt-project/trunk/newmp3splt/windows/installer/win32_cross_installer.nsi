!include MUI2.nsh

;program variables
!define VERSION "2.2.2"
!define PROGRAM_NAME "mp3splt"
!define MP3SPLT_PATH /home/ion/hacking/mp3splt-project/mp3splt-project/trunk/newmp3splt/windows/installer/../../..
!define PROGRAM_NAME_VERSION "${VERSION} v. ${VERSION}"

;name of the program
Name "mp3splt ${VERSION}"
;file to write
OutFile "mp3splt_${VERSION}.exe"
;installation directory
InstallDir $PROGRAMFILES\mp3splt

BrandingText " "

;interface settings
;!define MUI_ABORTWARNING
!define MUI_ICON ${MP3SPLT_PATH}/newmp3splt/windows/mp3splt.ico
!define MUI_UNICON ${MP3SPLT_PATH}/newmp3splt/windows/mp3splt.ico
!define MUI_COMPONENTSPAGE_NODESC
ShowInstDetails "show"
ShowUninstDetails "show"
SetCompressor /SOLID "lzma"

;install pages
!insertmacro MUI_PAGE_LICENSE ${MP3SPLT_PATH}\newmp3splt\COPYING
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

;uninstall pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;interface languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Spanish"


;main installation section
Section "mp3splt (with libmp3splt)" main_section
  SetOutPath "$INSTDIR"

  ;the main executable
  File ${MP3SPLT_PATH}\newmp3splt\src\mp3splt.exe
  ;the library
  File ${MP3SPLT_PATH}\libmp3splt\src\.libs\libmp3splt-0.dll
  ;the dependencies
  File ${MP3SPLT_PATH}\libltdl3.dll
  File ${MP3SPLT_PATH}\zlib1.dll

  FileOpen $9 mp3splt.bat w
  FileWrite $9 'cmd /K "cd $INSTDIR & .\mp3splt.exe"'
  FileClose $9

  WriteUninstaller "${PROGRAM_NAME}_uninst.exe"

  WriteRegStr HKLM "Software\${PROGRAM_NAME}\" "UninstallString" \
    "$INSTDIR\${PROGRAM_NAME}_uninst.exe"

  ;add to Add/Remove programs
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3splt" \
    "mp3splt" "mp3splt"
SectionEnd

;main plugins section
SubSection /e "Plugins" plugins_section

  Section "mp3 plugin" mp3_plugin_section
    File ${MP3SPLT_PATH}\libmad-0.dll
    File ${MP3SPLT_PATH}\libid3tag.dll
    File ${MP3SPLT_PATH}\libmp3splt\plugins\.libs\libsplt_mp3-0.dll
  SectionEnd

  Section "ogg vorbis plugin" ogg_plugin_section
    File ${MP3SPLT_PATH}\libogg-0.dll
    File ${MP3SPLT_PATH}\libvorbis-0.dll
    File ${MP3SPLT_PATH}\libvorbisenc-2.dll
    File ${MP3SPLT_PATH}\libvorbisfile-3.dll
    File ${MP3SPLT_PATH}\libmp3splt\plugins\.libs\libsplt_ogg-0.dll
  SectionEnd

SubSectionEnd

;main documentation section
SubSection /e "Documentation" documentation_section

  Section "mp3splt documentation" mp3splt_doc_section
    CreateDirectory "$INSTDIR\mp3splt_doc"
    SetOutPath $INSTDIR\mp3splt_doc
    File ${MP3SPLT_PATH}\newmp3splt\README
    File ${MP3SPLT_PATH}\newmp3splt\doc\manual.html
    File ${MP3SPLT_PATH}\newmp3splt\COPYING
    File ${MP3SPLT_PATH}\newmp3splt\ChangeLog
    File ${MP3SPLT_PATH}\newmp3splt\INSTALL
    File ${MP3SPLT_PATH}\newmp3splt\NEWS
    File ${MP3SPLT_PATH}\newmp3splt\TODO
    File ${MP3SPLT_PATH}\newmp3splt\AUTHORS
  SectionEnd

  Section "libmp3splt documentation" libmp3splt_doc_section
    CreateDirectory "$INSTDIR\libmp3splt_doc"
    SetOutPath $INSTDIR\libmp3splt_doc
    File ${MP3SPLT_PATH}\libmp3splt\README
    File ${MP3SPLT_PATH}\libmp3splt\COPYING
    File ${MP3SPLT_PATH}\libmp3splt\ChangeLog
    File ${MP3SPLT_PATH}\libmp3splt\INSTALL
    File ${MP3SPLT_PATH}\libmp3splt\NEWS
    File ${MP3SPLT_PATH}\libmp3splt\TODO
    File ${MP3SPLT_PATH}\libmp3splt\AUTHORS
  SectionEnd

SubSectionEnd


;start Menu Shortcuts section
Section "Start Menu Shortcuts" menu_shortcuts_section
  CreateDirectory "$SMPROGRAMS\mp3splt"
  CreateShortCut "$SMPROGRAMS\mp3splt\Mp3splt.lnk" "$INSTDIR\mp3splt.bat" "" ""
	CreateShortCut "$SMPROGRAMS\mp3splt\uninstall.lnk" "$INSTDIR\mp3splt_uninst.exe" "" "$INSTDIR\mp3splt_uninst.exe" 0

  ;if mp3splt_doc_section is selected, add mp3splt_doc link
  SectionGetFlags ${mp3splt_doc_section} $0
  IntOp $1 $0 & ${SF_SELECTED}
  IntCmp 0 $1 after_doc_shortcut
  CreateShortCut "$SMPROGRAMS\mp3splt\mp3splt_doc.lnk" "$INSTDIR\mp3splt_doc" "" "$INSTDIR\mp3splt_doc" 
  after_doc_shortcut:

  ;if libmp3splt_doc_section is selected, add libmp3splt_doc link
  SectionGetFlags ${libmp3splt_doc_section} $0
  IntOp $1 $0 & ${SF_SELECTED}
  IntCmp 0 $1 after_lib_doc_shortcut
  CreateShortCut "$SMPROGRAMS\mp3splt\libmp3splt_doc.lnk" "$INSTDIR\libmp3splt_doc"	"" "$INSTDIR\libmp3splt_doc" 
  after_lib_doc_shortcut:

SectionEnd


;desktop shortcut
Section "Desktop Shortcut" desktop_shortcut_section
	CreateShortCut "$DESKTOP\mp3splt.lnk" "$INSTDIR\mp3splt.bat" "" ""  
SectionEnd


;uninstallation section
Section "Uninstall"

  ;;main_section
  ;the main executable
  Delete $INSTDIR\mp3splt.exe
  Delete $INSTDIR\mp3splt.bat
  ;the library
  Delete $INSTDIR\libmp3splt-0.dll
  ;the dependencies
  Delete $INSTDIR\libltdl3.dll
  Delete $INSTDIR\zlib1.dll

  ;;plugins_section
  ;mp3 plugin
  Delete $INSTDIR\libmad-0.dll
  Delete $INSTDIR\libid3tag.dll
  Delete $INSTDIR\libsplt_mp3-0.dll

  ;ogg plugin
  Delete $INSTDIR\libogg-0.dll
  Delete $INSTDIR\libvorbis-0.dll
  Delete $INSTDIR\libvorbisenc-2.dll
  Delete $INSTDIR\libvorbisfile-3.dll
  Delete $INSTDIR\libsplt_ogg-0.dll

  ;;documentation section
  Delete $INSTDIR\mp3splt_doc\manual.html
  Delete $INSTDIR\mp3splt_doc\README
  Delete $INSTDIR\mp3splt_doc\COPYING
  Delete $INSTDIR\mp3splt_doc\ChangeLog
  Delete $INSTDIR\mp3splt_doc\INSTALL
  Delete $INSTDIR\mp3splt_doc\NEWS
  Delete $INSTDIR\mp3splt_doc\TODO
  Delete $INSTDIR\mp3splt_doc\AUTHORS
  RmDir $INSTDIR\mp3splt_doc
  Delete "$SMPROGRAMS\mp3splt\mp3splt_doc.lnk"

  ;libmp3splt_doc
  Delete $INSTDIR\libmp3splt_doc\README
  Delete $INSTDIR\libmp3splt_doc\COPYING
  Delete $INSTDIR\libmp3splt_doc\ChangeLog
  Delete $INSTDIR\libmp3splt_doc\INSTALL
  Delete $INSTDIR\libmp3splt_doc\NEWS
  Delete $INSTDIR\libmp3splt_doc\TODO
  Delete $INSTDIR\libmp3splt_doc\AUTHORS
  RmDir $INSTDIR\libmp3splt_doc
  Delete "$SMPROGRAMS\mp3splt\libmp3splt_doc.lnk"

  ;;menu shortcuts section
  Delete "$SMPROGRAMS\mp3splt\Mp3splt.lnk"
  Delete "$SMPROGRAMS\mp3splt\uninstall.lnk"
  RMDir "$SMPROGRAMS\mp3splt"

  ;;desktop shortcut section
  Delete "$DESKTOP\mp3splt.lnk"

  ;delete remaining ashes if possible
  Delete $INSTDIR\${PROGRAM_NAME}_uninst.exe
  RmDir $INSTDIR\${PROGRAM_NAME}

  ;delete registry
  DeleteRegKey HKLM "Software\${PROGRAM_NAME}"

SectionEnd


;uninstall the old program if necessary
Function .onInit

  ;read only and select the main section
  IntOp $0 ${SF_SELECTED} | ${SF_RO}
  SectionSetFlags ${main_section} $0

  ;read from registry eventual uninstall string
  ReadRegStr $R0 HKLM "Software\${PROGRAM_NAME}\" "UninstallString"
  StrCmp $R0 "" done
 
  ;uninstall previous installation
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
  "${PROGRAM_NAME} is already installed. $\n$\nClick `OK` to remove the \
  previous installation or `Cancel` to cancel this installation." \
  IDOK uninst
  Abort

  ;run the uninstaller
  uninst:
    ClearErrors
    ExecWait '$R0 _?=$INSTDIR'
    IfErrors no_remove_uninstaller
    no_remove_uninstaller:
  done:

FunctionEnd


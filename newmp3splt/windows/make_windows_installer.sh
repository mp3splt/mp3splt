#!/bin/sh

#if the first argument of the script is not empty, then we consider a cross
#compilation; otherwise, consider a windows installation
we_dont_cross_compile=$1

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

. ../include_variables.sh

ARCH="i386"

if [[ -z $we_dont_cross_compile ]];then
  put_package "windows_installer"
  DLL_SUFFIX=""
else
  put_package "cross_windows_installer"
  DLL_SUFFIX="-0"
fi

#generate the '.nsi' installer script

WIN_INSTALLER_FILE="win32_installer.nsi"

TMP_GENERATED_FILES_FILE='.mp3splt_tmp_uninstall_files.txt'
TMP_CREATED_DIRECTORIES_FILE='.mp3splt_tmp_uninstall_directories.txt'

echo '' > $TMP_GENERATED_FILES_FILE
echo '' > $TMP_CREATED_DIRECTORIES_FILE
 

CURRENT_OUT_PATH=""

function set_out_path()
{
  out_path=$1
  CURRENT_OUT_PATH=$out_path
  echo '    SetOutPath '$out_path >> $WIN_INSTALLER_FILE
}

#-generate function files and keeps the files in a temporary files for
#generating the uninstall section
#-requires as parameter a list of files
function copy_files()
{
  FILES=$@
  for file in $FILES;do
    echo '    File ${MP3SPLT_PATH}'"\\$file" >> $WIN_INSTALLER_FILE
    file_without_path=${file##*\\}
    echo '  Delete '"$CURRENT_OUT_PATH\\$file_without_path" >> $TMP_GENERATED_FILES_FILE
  done
}

#-function that creates a directory with NSI commands and keeps the
#directories in a temporary file for generating the uninstaller
function create_directory()
{
  DIR=$@
  echo '    CreateDirectory '$DIR >> $WIN_INSTALLER_FILE
  echo "  RmDir $DIR" >> $TMP_CREATED_DIRECTORIES_FILE
}

#main options
echo "!include MUI2.nsh

;program variables
!define VERSION \"$MP3SPLT_VERSION\"
!define PROGRAM_NAME \"mp3splt\"" > $WIN_INSTALLER_FILE

if [[ -z $we_dont_cross_compile ]];then
  echo "!define MP3SPLT_PATH \"c:/mp3splt_mingw`pwd`/../../..\"" >> $WIN_INSTALLER_FILE
else 
  echo "!define MP3SPLT_PATH \"`pwd`/../..\"" >> $WIN_INSTALLER_FILE
fi


#generates the Delete file1.txt for all the installed files
# and after, RmDir dir for all the created directories
function generate_uninstall_files_dirs()
{
  cat $TMP_GENERATED_FILES_FILE >> $WIN_INSTALLER_FILE
  cat $TMP_CREATED_DIRECTORIES_FILE >> $WIN_INSTALLER_FILE
}

echo '!define PROGRAM_NAME_VERSION "${VERSION} v. ${VERSION}"

;name of the program
Name "mp3splt ${VERSION}"
;file to write
OutFile "../../mp3splt_'$MP3SPLT_VERSION'_'$ARCH'.exe"
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
!insertmacro MUI_LANGUAGE "Spanish"' >> $WIN_INSTALLER_FILE

#main section
MAIN_SECTION_FILES="
newmp3splt\src\mp3splt.exe
libmp3splt\src\.libs\libmp3splt${DLL_SUFFIX}.dll
libltdl3.dll
zlib1.dll
"

echo '
;main installation section
Section "mp3splt (with libmp3splt)" main_section
' >> $WIN_INSTALLER_FILE

set_out_path '$INSTDIR'
copy_files $MAIN_SECTION_FILES

echo '
  FileOpen $9 mp3splt.bat w
  FileWrite $9 '\''cmd /K "cd $INSTDIR & .\mp3splt.exe"'\''
  FileClose $9

  WriteUninstaller "${PROGRAM_NAME}_uninst.exe"

  WriteRegStr HKLM "Software\${PROGRAM_NAME}\" "UninstallString" \
    "$INSTDIR\${PROGRAM_NAME}_uninst.exe"

  ;add to Add/Remove programs
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME}" \
    "DisplayName" "mp3splt"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME}" \
    "UninstallString" "$INSTDIR\${PROGRAM_NAME}_uninst.exe"

SectionEnd' >> $WIN_INSTALLER_FILE


#mp3 plugin section
MP3_PLUGIN_FILES="
libmad-0.dll
libid3tag.dll
libmp3splt\plugins\.libs\libsplt_mp3${DLL_SUFFIX}.dll
"

echo '
;main plugins section
SubSection /e "Plugins" plugins_section

  Section "mp3 plugin" mp3_plugin_section
' >> $WIN_INSTALLER_FILE

set_out_path '$INSTDIR'
copy_files $MP3_PLUGIN_FILES

echo '
  SectionEnd
' >> $WIN_INSTALLER_FILE


#ogg plugin section
OGG_PLUGIN_FILES="
libogg-0.dll
libvorbis-0.dll
libvorbisenc-2.dll
libvorbisfile-3.dll
libmp3splt\plugins\.libs\libsplt_ogg${DLL_SUFFIX}.dll
"

echo '  Section "ogg vorbis plugin" ogg_plugin_section
' >> $WIN_INSTALLER_FILE

set_out_path '$INSTDIR'
copy_files $OGG_PLUGIN_FILES

echo '
  SectionEnd

SubSectionEnd' >> $WIN_INSTALLER_FILE


#mp3splt doc section
MP3SPLT_DOC_FILES="
newmp3splt\README
newmp3splt\doc\manual.html
newmp3splt\COPYING
newmp3splt\ChangeLog
newmp3splt\INSTALL
newmp3splt\NEWS
newmp3splt\TODO
newmp3splt\AUTHORS
"

echo '
;main documentation section
SubSection /e "Documentation" documentation_section

  Section "mp3splt documentation" mp3splt_doc_section
' >> $WIN_INSTALLER_FILE

create_directory '$INSTDIR\mp3splt_doc'
set_out_path '$INSTDIR\mp3splt_doc'
copy_files $MP3SPLT_DOC_FILES

echo '
  SectionEnd' >> $WIN_INSTALLER_FILE


#libmp3splt doc section
LIBMP3SPLT_DOC_FILES="
libmp3splt\README
libmp3splt\COPYING
libmp3splt\ChangeLog
libmp3splt\INSTALL
libmp3splt\NEWS
libmp3splt\TODO
libmp3splt\AUTHORS
"

echo '
  Section "libmp3splt documentation" libmp3splt_doc_section
' >> $WIN_INSTALLER_FILE

create_directory '$INSTDIR\libmp3splt_doc'
set_out_path '$INSTDIR\libmp3splt_doc'
copy_files $LIBMP3SPLT_DOC_FILES

echo '
  SectionEnd

SubSectionEnd' >> $WIN_INSTALLER_FILE


echo '
;start Menu Shortcuts section
Section "Start Menu Shortcuts" menu_shortcuts_section
' >> $WIN_INSTALLER_FILE

create_directory '$SMPROGRAMS\mp3splt'

echo '  CreateShortCut "$SMPROGRAMS\mp3splt\Mp3splt.lnk" "$INSTDIR\mp3splt.bat" "" ""
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

SectionEnd' >> $WIN_INSTALLER_FILE


echo '
;uninstallation section
Section "Uninstall"

  Delete $INSTDIR\mp3splt.bat' >> $WIN_INSTALLER_FILE

generate_uninstall_files_dirs

echo '
  ;menu shortcuts
  Delete $SMPROGRAMS\mp3splt\mp3splt_doc.lnk
  Delete $SMPROGRAMS\mp3splt\libmp3splt_doc.lnk
  Delete $SMPROGRAMS\mp3splt\Mp3splt.lnk
  Delete $SMPROGRAMS\mp3splt\uninstall.lnk

  ;desktop shortcut section
  Delete $DESKTOP\mp3splt.lnk

  ;delete remaining ashes if possible
  Delete $INSTDIR\${PROGRAM_NAME}_uninst.exe
  RmDir $INSTDIR

  ;delete registry
  DeleteRegKey HKLM Software\${PROGRAM_NAME}
  DeleteRegKey HKLM Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME}

SectionEnd' >> $WIN_INSTALLER_FILE


#uninstall old program if found installed
echo '
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
    ExecWait '\''$R0 _?=$INSTDIR'\''
    IfErrors no_remove_uninstaller
    no_remove_uninstaller:
  done:

FunctionEnd
' >> $WIN_INSTALLER_FILE


rm -f $TMP_GENERATED_FILES_FILE
rm -f $TMP_CREATED_DIRECTORIES_FILE

if [[ -z $we_dont_cross_compile ]];then
  ../../../nsis/makensis win32_installer.nsi || exit 1
else
  makensis win32_installer.nsi || exit 1
fi


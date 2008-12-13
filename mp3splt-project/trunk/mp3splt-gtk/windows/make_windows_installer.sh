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

#we compile the locales
mkdir -p ../../fr_locales
wine `pwd`/../../../libs/bin/msgfmt -o ../../fr_locales/mp3splt-gtk.mo ../po/fr.po || exit 1

cd ../../../libs &&\
tar jxf mp3splt-gtk_runtime.tar.bz2 -C ../trunk || exit 1 &&\
cd -


#generate the '.nsi' installer script

WIN_INSTALLER_FILE="win32_installer.nsi"

TMP_GENERATED_FILES_FILE='.mp3splt-gtk_tmp_uninstall_files.txt'
TMP_CREATED_DIRECTORIES_FILE='.mp3splt-gtk_tmp_uninstall_directories.txt'

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
  DIR=$1
  echo '    CreateDirectory '$DIR >> $WIN_INSTALLER_FILE
  echo "  RmDir $DIR" >> $TMP_CREATED_DIRECTORIES_FILE
}

#-generate function files recursively from the director passed as parameter
function recursive_copy_files_from_directory()
{
  DIR=$1

  cd $DIR

  directories=$(find . -type d)
  files=$(find . -type f)

  #prepare directories to remove at the uninstall
  find . -type d -exec echo '  RmDir ''{}' \; | grep -v 'RmDir \.$' | \
    sed 's+RmDir \.+RmDir $INSTDIR+; s+/+\\+g' | tac >> $script_dir/$TMP_CREATED_DIRECTORIES_FILE

  #prepare files to remove at the uninstall
  find . -type f -exec echo '  Delete $INSTDIR'"\\"'{}' \; | \
    sed 's+$INSTDIR\\\.+$INSTDIR+; s+/+\\+g' >> $script_dir/$TMP_GENERATED_FILES_FILE

  cd -

  echo '' >> $WIN_INSTALLER_FILE

  #create the directories
  for cur_dir in ${directories};do

    windows_dir=$(echo ${cur_dir#\.\/} | sed 's+/+\\+g')
    windows_dir="\\"$windows_dir

    #prepare windows_dir as '\directories' or empty string
    if [[ $windows_dir = "\\." ]];then
      windows_dir=""
    fi

    #don't re-create the $INSTDIR directory
    if [[ ! -z $windows_dir ]];then
      echo '    CreateDirectory $INSTDIR'$windows_dir >> $WIN_INSTALLER_FILE
    fi
    
    #copy files from the current directory (not recursively)
    cd $DIR/$cur_dir
    files=$(find . -maxdepth 1 -type f)
    cd -

    if [[ ! -z $files ]];then

      #set the outpath as the current directory
      set_out_path '$INSTDIR'$windows_dir

      for cur_file in ${files};do
        file=$(echo ${cur_file#\.\/} | sed 's+/+\\+g')
        echo '    File ${MP3SPLT_PATH}\mp3splt-gtk_runtime'$windows_dir"\\"$file >> $WIN_INSTALLER_FILE
      done

    fi

  done
}

#main options
echo "!include MUI2.nsh

;program variables
!define VERSION \"${MP3SPLT_GTK_VERSION}\"
!define PROGRAM_NAME \"mp3splt-gtk\"" > $WIN_INSTALLER_FILE

if [[ -z $we_dont_cross_compile ]];then
  echo "!define MP3SPLT_PATH \"c:/mp3splt_mingw`pwd`/../..\"" >> $WIN_INSTALLER_FILE
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

echo '
;name of the program
Name "mp3splt-gtk ${VERSION}"
;file to write
OutFile "../../mp3splt-gtk_${VERSION}_'$ARCH'.exe"
;installation directory
InstallDir $PROGRAMFILES\mp3splt-gtk

BrandingText " "

;interface settings
!define MUI_ICON ${MP3SPLT_PATH}/mp3splt-gtk/windows/mp3splt-gtk.ico
!define MUI_UNICON ${MP3SPLT_PATH}/mp3splt-gtk/windows/mp3splt-gtk.ico
!define MUI_COMPONENTSPAGE_NODESC
ShowInstDetails "show"
ShowUninstDetails "show"
SetCompressor /SOLID "lzma"

;install pages
!insertmacro MUI_PAGE_LICENSE ${MP3SPLT_PATH}\mp3splt-gtk\COPYING
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
mp3splt-gtk\src\mp3splt-gtk.exe
libmp3splt\src\.libs\libmp3splt${DLL_SUFFIX}.dll
libltdl3.dll
mp3splt-gtk\src\mp3splt-gtk.png
mp3splt-gtk\src\mp3splt-gtk_ico.png
mp3splt-gtk\windows\mp3splt-gtk.ico
"

echo '
;main installation section
Section "mp3splt-gtk (with libmp3splt, gtk & gstreamer)" main_section
' >> $WIN_INSTALLER_FILE

set_out_path '$INSTDIR'
copy_files $MAIN_SECTION_FILES
recursive_copy_files_from_directory "../../mp3splt-gtk_runtime"

echo '
  WriteUninstaller "${PROGRAM_NAME}_uninst.exe"

  WriteRegStr HKLM "Software\${PROGRAM_NAME}\" "UninstallString" \
    "$INSTDIR\${PROGRAM_NAME}_uninst.exe"

  ;add to Add/Remove programs
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM_NAME}" \
    "DisplayName" "mp3splt-gtk"
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
mp3splt-gtk\README
mp3splt-gtk\COPYING
mp3splt-gtk\ChangeLog
mp3splt-gtk\INSTALL
mp3splt-gtk\NEWS
mp3splt-gtk\TODO
mp3splt-gtk\AUTHORS
"

echo '
;main documentation section
SubSection "Documentation" documentation_section

  Section "mp3splt-gtk documentation" mp3splt_gtk_doc_section
' >> $WIN_INSTALLER_FILE

create_directory '$INSTDIR\mp3splt-gtk_doc'
set_out_path '$INSTDIR\mp3splt-gtk_doc'
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


#translations section
echo '
SubSection "Translations" translations_section
 
  Section "English" english_translation_section 

  SectionEnd

  Section "French" french_translation_section

    CreateDirectory $INSTDIR\translations\fr\LC_MESSAGES
    SetOutPath $INSTDIR\translations\fr\LC_MESSAGES
    File ${MP3SPLT_PATH}\fr_locales\mp3splt-gtk.mo

  SectionEnd

SubSectionEnd
' >> $WIN_INSTALLER_FILE


echo '
;start Menu Shortcuts section
Section "Start Menu Shortcuts" menu_shortcuts_section
' >> $WIN_INSTALLER_FILE

create_directory '$SMPROGRAMS\mp3splt-gtk'

echo '  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\mp3splt-gtk.lnk" "$INSTDIR\mp3splt-gtk.exe" "" "$INSTDIR\mp3splt-gtk.ico" 0
  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\Uninstall.lnk" "$INSTDIR\mp3splt-gtk_uninst.exe" "" "$INSTDIR\mp3splt-gtk_uninst.exe" 0

  ;if mp3splt_gtk__doc_section is selected, add mp3splt_doc link
  SectionGetFlags ${mp3splt_gtk_doc_section} $0
  IntOp $1 $0 & ${SF_SELECTED}
  IntCmp 0 $1 after_doc_shortcut
  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\mp3splt-gtk_doc.lnk" "$INSTDIR\mp3splt-gtk_doc" "" "$INSTDIR\mp3splt-gtk_doc" 
  after_doc_shortcut:

  ;if libmp3splt_doc_section is selected, add libmp3splt_doc link
  SectionGetFlags ${libmp3splt_doc_section} $0
  IntOp $1 $0 & ${SF_SELECTED}
  IntCmp 0 $1 after_lib_doc_shortcut
  CreateShortCut "$SMPROGRAMS\mp3splt-gtk\libmp3splt_doc.lnk" "$INSTDIR\libmp3splt_doc"	"" "$INSTDIR\libmp3splt_doc" 
  after_lib_doc_shortcut:

SectionEnd

;desktop shortcut
Section "Desktop Shortcut" desktop_shortcut_section

	CreateShortCut "$DESKTOP\mp3splt-gtk.lnk" "$INSTDIR\mp3splt-gtk.exe" "" "$INSTDIR\mp3splt-gtk.ico"

SectionEnd' >> $WIN_INSTALLER_FILE


echo '
;uninstallation section
Section "Uninstall"' >> $WIN_INSTALLER_FILE

generate_uninstall_files_dirs

echo '
  ;menu shortcuts
  Delete $SMPROGRAMS\mp3splt-gtk\mp3splt-gtk_doc.lnk
  Delete $SMPROGRAMS\mp3splt-gtk\libmp3splt_doc.lnk
  Delete $SMPROGRAMS\mp3splt-gtk\Mp3splt-gtk.lnk
  Delete $SMPROGRAMS\mp3splt-gtk\Uninstall.lnk
  RmDir $SMPROGRAMS\mp3splt-gtk

  ;delete the translations
  Delete $INSTDIR\translations\fr\LC_MESSAGES\mp3splt-gtk.mo
  RmDir $INSTDIR\translations\fr\LC_MESSAGES
  RmDir $INSTDIR\translations\fr
  RmDir $INSTDIR\translations

  ;desktop shortcut section
  Delete $DESKTOP\mp3splt-gtk.lnk

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

  ;read only and select the english translation
  IntOp $0 ${SF_SELECTED} | ${SF_RO}
  SectionSetFlags ${english_translation_section} $0

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

#remove .nsi script
#rm -f $WIN_INSTALLER_FILE

#remove used dirs
rm -rf ../fr_locales && cd ../.. && rm -rf mp3splt-gtk_runtime


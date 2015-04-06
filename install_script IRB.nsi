# Set some constants
!define APPNAME "IRR RPG BUILDER"
!define DESCRIPTION "RPG Game editing tool (alpha 0.31)"
# These will be displayed by the "Click here for support information" link in "Add/Remove Programs"
# It is possible to use "mailto:" links in here to open the email client
!define HELPURL "http://irrrpgbuilder.sourceforge.net/forum/" # "Support Information" link
!define UPDATEURL "https://sourceforge.net/projects/irrrpgbuilder/files/?source=navbar" # "Product Updates" link
!define ABOUTURL "http://irrrpgbuilder.sourceforge.net/" # "Publisher" link
!define VERSIONMAJOR "0"
!define VERSIONMINOR "31"

# The icon used
!define ICONPATH "bin\irrlicht.ico"

RequestExecutionLevel admin ;Require admin rights on NT6+ (When UAC is turned on)

# set the compressor (this is for minimum size)
SetCompressor /SOLID lzma

InstallDir "$PROGRAMFILES\${APPNAME}"
 
# rtf or txt file - remember if it is txt, it must be in the DOS text format (\r\n)
LicenseText "IRR RPG BUILDER - Copyright notice" $(^NextBtn)
LicenseData "LICENSE.txt"
# This will be in the installer/uninstaller's title bar
Name "${APPNAME}"
Icon "${ICONPATH}"

# this is the name of the installer to be created
outFile "IRR_RPG_BUILDER_setup.exe"
 
# some libs for additional functions
!include LogicLib.nsh
 
# Just four pages - license agreement, components to install, install location, and installation
# all of them are pre-defined pages --> we don't have to define them in the install script
Page license
Page components
Page directory
Page instfiles

; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\German.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Spanish.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\PortugueseBR.nlf"
 
# i copied this macro from the web
!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
        messageBox mb_iconstop "Administrator rights required to write into the Program Files folder!"
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend

# check on the start of the installer for admin-rights
function .onInit
	setShellVarContext all
	!insertmacro VerifyUserIsAdmin
functionEnd


# each section is one part of the installation. This is the main section, it cannot be disabled (it's name start with '-')
section "-Install files"
	# Files for the install directory - to build the installer, these should be in the same directory as the install script (this file)
	# Files added here should be removed by the uninstaller (see section "uninstall")
	setOutPath $INSTDIR"
	file /x *.nsi /x *.vcproj /x *.sln *.*

	setOutPath $INSTDIR\bin"
	file bin\*.*
	
	setOutPath $INSTDIR\docs"
	file docs\*.*
	
	setOutPath $INSTDIR\media"
	file /r media media\*.*
	
	setOutPath $INSTDIR\projects"
	file /r projects projects\*.*
 
	# Uninstaller - See function un.onInit and section "uninstall" for configuration
	writeUninstaller "$INSTDIR\uninstall.exe"
	
	AccessControl::GrantOnFile "$INSTDIR\bin" "(BU)" "FullAccess"
	AccessControl::GrantOnFile "$INSTDIR\media" "(BU)" "FullAccess"
	AccessControl::GrantOnFile "$INSTDIR\project" "(BU)" "FullAccess"
 
	# Registry information for add/remove programs
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME} - ${DESCRIPTION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayIcon" "$\"$INSTDIR\bin\IrrRPG_Builder_win32.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "HelpLink" "$\"${HELPURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLUpdateInfo" "$\"${UPDATEURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayVersion" "$\"${VERSIONMAJOR}.${VERSIONMINOR}$\""
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMajor" ${VERSIONMAJOR}
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMinor" ${VERSIONMINOR}
	# There is no option for modifying or repairing the install
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
	setOutPath $INSTDIR"
sectionEnd

# this is the next section. It can be disabled (and omitted) in the components page.
section "Start menu items"
	# Start Menu
	setOutPath "$INSTDIR\bin"
	createDirectory "$SMPROGRAMS\${APPNAME}"
	createShortCut "$SMPROGRAMS\${APPNAME}\Editor.lnk" "$INSTDIR\bin\IrrRPG_Builder_win32.exe" "" ""
	createShortCut "$SMPROGRAMS\${APPNAME}\Player.lnk" "$INSTDIR\bin\IrrRPG_Builder_player_win32.exe" "" ""
	createShortCut "$SMPROGRAMS\${APPNAME}\Documentation.lnk" "$INSTDIR\docs\IRB_userGuide.pdf" "" ""
	createShortCut "$SMPROGRAMS\${APPNAME}\LUA Documentation.lnk" "$INSTDIR\docs\Lua_command_reference.pdf" "" ""
	createShortCut "$SMPROGRAMS\${APPNAME}\uninstall.lnk" "$INSTDIR\uninstall.exe" "" "" 
sectionEnd
 
# this is the next section. It can be disabled (and omitted) in the components page.
section "Desktop icons"
	# Start Menu
	setOutPath "$INSTDIR\bin"
	createShortCut "$DESKTOP\IRB Editor.lnk" "$INSTDIR\bin\IrrRPG_Builder_win32.exe" "" ""
	createShortCut "$DESKTOP\IRB Player.lnk" "$INSTDIR\bin\IrrRPG_Builder_player_win32.exe" "" ""
sectionEnd

section "Source files"
	# Files for the install directory - to build the installer, these should be in the same directory as the install script (this file)
	setOutPath $INSTDIR"	
	file *.sln *.vcproj
	
	setOutPath $INSTDIR\irrKlang"
	file /r irrKlang irrKlang\*.*
	
	setOutPath $INSTDIR\irrlicht-engine"
	file /r irrlicht-engine  irrlicht-engine\*.*
	
	setOutPath $INSTDIR\src"
	file /r src src\*.*	
	
	AccessControl::GrantOnFile "$INSTDIR\" "(BU)" "FullAccess"
	AccessControl::GrantOnFile "$INSTDIR\irrlicht-engine" "(BU)" "FullAccess"
	AccessControl::GrantOnFile "$INSTDIR\src" "(BU)" "FullAccess"
	
	setOutPath $INSTDIR"	
sectionEnd
 
# Uninstaller
# this is almost the same as the installer. Here is the initial question.
function un.onInit
	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Permanently remove ${APPNAME}?" IDOK next
		Abort
	next:
	!insertmacro VerifyUserIsAdmin
functionEnd
 
# and here we have the section, that uninstalls everything
section "uninstall"
 
	# Remove Start Menu launcher
	delete "$SMPROGRAMS\${APPNAME}\Editor.lnk"
	delete "$SMPROGRAMS\${APPNAME}\Player.lnk"
	delete "$SMPROGRAMS\${APPNAME}\Documentation.lnk"
	delete "$SMPROGRAMS\${APPNAME}\LUA Documentation.lnk"
	delete "$SMPROGRAMS\${APPNAME}\uninstall.lnk"
	# Try to remove the Start Menu folder - this will only happen if it is empty
	RMDir "$SMPROGRAMS\${APPNAME}"

	delete "$DESKTOP\IRB Editor.lnk"
	delete "$DESKTOP\IRB Player.lnk"
	
	# Remove files
	# there is the advice in the web, that you don't should do simply 'delete "$INSTDIR\*.*"'. There might be people, that 
	# install directly into "C:\Program Files" or "D:\Games". So you would delete way too much with "*.*".
 	delete "$INSTDIR\*.xml"
	ClearErrors
	RMDir /r "$INSTDIR\bin"
	RMDir /r "$INSTDIR\docs"
	RMDir /r "$INSTDIR\media"
	RMDir /r "$INSTDIR\projects"
	
	RMDir /r "$INSTDIR\irrKlang"
	RMDir /r "$INSTDIR\irrlicht-engine"
	RMDir /r "$INSTDIR\src"
	
 	delete "$INSTDIR\*.txt"
 	delete "$INSTDIR\*.sln"
	delete "$INSTDIR\*.vcproj"
 	

	# Always delete uninstaller as the last action
	delete $INSTDIR\uninstall.exe
 
	# Try to remove the install directory - this will only happen if it is empty
	rmDir $INSTDIR
 
	# Remove uninstaller information from the registry
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
sectionEnd

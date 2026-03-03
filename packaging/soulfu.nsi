; soulfu.nsi - NSIS script for SoulFu installer

; Define installer name and output file
!ifdef OUTFILE
OutFile "${OUTFILE}"
!else
OutFile "soulfu_win32_${VERSION}.exe"
!endif
Name "SoulFu"
!ifdef INSTDIR_NAME
InstallDir "$PROGRAMFILES64\SoulFu"
!else
InstallDir "$PROGRAMFILES\SoulFu"
!endif

; Request administrator privileges for installation
RequestExecutionLevel admin

; Add an icon to the installer
Icon "Chest.ico"

; Define the installer pages
Page license
Page directory
Page instfiles

; Define the uninstaller pages
UninstPage uninstConfirm
UninstPage instfiles

; License agreement
LicenseData "license.txt"

; Define the installation section
Section "Install"
    ; Create the installation directory
    SetOutPath "$INSTDIR"

    ; Add the application files
    File "soulfu.exe"
    File "datafile.sdf"
    File "Manual.htm"
    File "SDL2.dll"
    File "SDL2_net.dll"
    File "libogg-0.dll"
    File "libvorbis-0.dll"
    File "Chest.ico"

    ; Create a shortcut in the Start Menu
    CreateDirectory "$SMPROGRAMS\SoulFu"
    CreateShortcut "$SMPROGRAMS\SoulFu\SoulFu.lnk" "$INSTDIR\soulfu.exe" "" "$INSTDIR\Chest.ico"
    CreateShortcut "$SMPROGRAMS\SoulFu\Manual.lnk" "$INSTDIR\Manual.htm"

    ; Create a desktop shortcut (optional)
    CreateShortcut "$DESKTOP\SoulFu.lnk" "$INSTDIR\soulfu.exe" "" "$INSTDIR\Chest.ico"

    ; Write uninstaller information
    WriteUninstaller "$INSTDIR\Uninstall.exe"

    ; Add registry entry for uninstaller
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "DisplayName" "SoulFu"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "UninstallString" '"$INSTDIR\Uninstall.exe"'
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "DisplayIcon" '"$INSTDIR\Chest.ico"'
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "Publisher" "${PUBLISHER}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "DisplayVersion" "${VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "URLInfoAbout" "${HOMEPAGE}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "HelpLink" "${HOMEPAGE}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "URLUpdateInfo" "${HOMEPAGE}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu" "NoRepair" 1
SectionEnd

; Define the uninstallation section
Section "Uninstall"
    ; Remove installed files
    Delete "$INSTDIR\soulfu.exe"
    Delete "$INSTDIR\datafile.sdf"
    Delete "$INSTDIR\Manual.htm"
    Delete "$INSTDIR\SDL2.dll"
    Delete "$INSTDIR\SDL2_net.dll"
    Delete "$INSTDIR\libogg-0.dll"
    Delete "$INSTDIR\libvorbis-0.dll"
    Delete "$INSTDIR\Chest.ico"
    Delete "$INSTDIR\Uninstall.exe"

    ; Remove shortcuts
    Delete "$SMPROGRAMS\SoulFu\SoulFu.lnk"
    Delete "$SMPROGRAMS\SoulFu\Manual.lnk"
    RMDir "$SMPROGRAMS\SoulFu"

    ; Remove desktop shortcut (optional)
    Delete "$DESKTOP\SoulFu.lnk"

    ; Remove the installation directory
    RMDir "$INSTDIR"

    ; Remove registry entry for uninstaller
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SoulFu"
SectionEnd

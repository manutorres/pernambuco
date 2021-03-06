; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "KPM Portfolio"
#define MyAppVersion "1.0"
#define MyAppPublisher "KPM"
#define MyAppURL "http://kidsplaymath.org/"
#define MyAppExeName "portfolio.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{8988F505-9754-4ADA-9994-C48F9C300373}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=..\..\license\lgpl-3.0.txt
OutputDir=.
OutputBaseFilename=Setup KPM Portfolio
SetupIconFile= icon.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "..\..\bin\portfolio.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\freetype6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\jpeg62.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\libgcrypt-11.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\libgpg-error-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\libjpeg-7.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\libmysql.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\libpng3.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\libpodofo.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\libssh2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\libtiff3.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\mingwm10.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\qsqlmysql4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\QtCore4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\QtGui4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\QtNetwork4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\QtSql4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\QtWebKit4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\QtXml4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\zlib1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\bin\sqldrivers\*"; DestDir: "{app}\sqldrivers"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent


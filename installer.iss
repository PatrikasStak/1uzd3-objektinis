[Setup]
AppName=Studentu Valdymo Sistema
AppVersion=3.0
AppPublisher=Patrikas Stakenas
AppPublisherURL=https://github.com/PatrikasStak/1uzd3-objektinis
AppSupportURL=https://github.com/PatrikasStak/1uzd3-objektinis
DefaultDirName={commonpf}\VU\Patrikas-Stakenas
DefaultGroupName=VU\Patrikas-Stakenas
PrivilegesRequired=admin
OutputDir=installer_output
OutputBaseFilename=Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "main.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Studentu Valdymo Sistema"; Filename: "{app}\main.exe"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Studentu Valdymo Sistema"; Filename: "{app}\main.exe"

[Run]
Filename: "{app}\main.exe"; Description: "Launch application"; Flags: nowait postinstall skipifsilent

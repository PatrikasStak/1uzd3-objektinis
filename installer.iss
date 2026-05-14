[Setup]
AppName=Studentu Valdymo Sistema
AppVersion=3.0
AppPublisher=Patrikas Stakenas
AppPublisherURL=https://github.com/PatrikasStak/1uzd3-objektinis
AppSupportURL=https://github.com/PatrikasStak/1uzd3-objektinis
DefaultDirName={autopf}\StudentuValdymo
DefaultGroupName=Studentu Valdymo Sistema
OutputDir=installer_output
OutputBaseFilename=Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional icons:"

[Files]
Source: "main.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Studentu Valdymo Sistema"; Filename: "{app}\main.exe"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Studentu Valdymo Sistema"; Filename: "{app}\main.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\main.exe"; Description: "Launch application"; Flags: nowait postinstall skipifsilent

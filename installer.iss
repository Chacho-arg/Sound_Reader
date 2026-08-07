[Setup]
AppName=SoundReader
AppVersion=1.0.0
DefaultDirName={pf}\SoundReader
DefaultGroupName=SoundReader
OutputDir=.
OutputBaseFilename=SoundReaderInstaller
Compression=lzma2
SolidCompression=yes

[Files]
Source: "build\\SoundReader.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\\SoundReader.vst3"; DestDir: "{app}\VST3"; Flags: ignoreversion

[Icons]
Name: "{group}\SoundReader"; Filename: "{app}\SoundReader.exe"
Name: "{group}\SoundReader VST3"; Filename: "{app}\VST3\SoundReader.vst3"

[Run]
Filename: "{app}\SoundReader.exe"; Description: "Launch SoundReader"; Flags: nowait postinstall skipifsilent

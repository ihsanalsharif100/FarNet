# FarNet

this project lets users stream their windows device into a web browser or anything if they want

very fast:if it can run on an i5 from 2012 and 8gb ddr3 then it can run on anything

very customizable:you can control everything from the port to the quilty from a txt file

# how to compile

# 1.screemstream

## 1.download boost library
you will find instructions down

## 2.add this to the linker librares
```LinkerLib
mswsock
ws2_32
gdi32
ole32
user32
gdiplus
```
or this in the other linker settings
```OtherLinkerSettings
-lgdi32
-lmswsock
-lws2_32
-lole32
-luser32
-lgdiplus
```

## 3.add this to the project search directory
```
in build options -> search directory -> Compiler
C:\c++\lib\boost
```

# 2.soundstream (Farnet folder)
## 1.download boost
1.you don't need to do this if you already downloaded boost for the screenshot file

## 2.add this to the linker librares
```LinkerLib
mswsock
ws2_32
```
or this in the other linker settings
```OtherLinkerSettings
-lmswsock
-lws2_32
```

## 3.add this to the project search directory
```
in build options -> search directory -> Compiler
C:\c++\lib\boost
```
# mouse and keyboard control (RIMOUSE V1 and RECIVEINPUT V1 folders)

## 1. add boost to the project search directory (compiler)

## 2. add -lws2_32 to your other linker options or ws2_32 to your linker libraries 

# how to download boost
open: https://www.boost.org/releases/latest/

download: whatever your OS is.zip

what to do :download the zip and unzip it into C:\c++\lib\ or wherever path you want

or you could run in the cmd this
```bash
color 2 & ::not optional
git clone https://github.com/microsoft/vcpkg & :: download vcpkg and you need git
cd vcpkg
.\bootstrap-vcpkg.bat
vcpkg install boost:x64-mingw-static
```
## 2.put this in Include directory
```path
C:\path to vcpkg\installed\x64-mingw-static\include
```
## 3.and this in the Library directory
```path
C:\path to vcpkg\installed\x64-mingw-static\lib
```

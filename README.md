<img width="100%" height="auto" alt="FarNet" src="https://github.com/user-attachments/assets/b6b8ebca-21bf-420a-846a-7e7e3153a2cb" />

# FarNet

FarNet is a high-performance C++ application for streaming a Windows screen and audio to a web browser, with remote control capabilities.

## Designed to be lightweight, fast, and highly customizable.

# Features

## Screen & Audio Streaming
Real-time screen streaming to browser clients
Audio streaming support
Optimized for low-end systems (runs on older hardware like i5 + DDR3)

## Remote Control
Mouse control (synchronized positioning)
Keyboard input support
Enables full remote interaction (similar to TeamViewer / AnyDesk)

## Customization
Configurable via text files
Control ports, quality, and behavior


# Raw Steps to build
# what you need
1. CodeBlocks - completely optional
2. C++ compiler - use gnu for the instructions but you can still use any other
3. boost you can download it via vcpkg or by this way (https://github.com/ihsanalsharif100/FarNet/blob/V2.0.0/README.md#how-to-download-boost)

# how to compile

this section gives instructions on how to compile the code correctly

## 1.screemstream

### 1. add this to the linker librares
```LinkerLib
mswsock
ws2_32
gdi32
ole32
user32
gdiplus
```
#### or this in the other linker settings
```OtherLinkerSettings
-lgdi32
-lmswsock
-lws2_32
-lole32
-luser32
-lgdiplus
```

### 2. add this to the project search directory
```
in build options -> search directory -> Compiler
C:\c++\lib\boost
```

# 2. soundstream (Farnet folder)

## 1. add this to the linker librares
```LinkerLib
mswsock
ws2_32
```
or this in the other linker settings
```OtherLinkerSettings
-lmswsock
-lws2_32
```

## 2. add boost to the project search directory
```
in build options -> search directory -> Compiler
C:\the\directory\boost\
```

# mouse and keyboard control (RIMOUSE V1 and RECIVEINPUT V1 folders)

## 1. add boost to the project search directory (compiler)
```
in build options -> search directory -> Compiler
C:\the\directory\boost\
```

## 2. add the linker library
add
```
-lws2_32
```
to your other linker options or 
```
ws2_32
````
to your linker libraries 


# how to download boost
open: [https://www.boost.org/releases/latest/]

download: from the windows section get the version you want.zip (this project is on 1.90 so it is boost_1_90_0.zip)

what to do :download the zip and unzip it into C:\c++\lib\ or wherever path you want

or you could run in the cmd this
```bash
color 2 & ::this line is not optional
git clone https://github.com/microsoft/vcpkg & :: download vcpkg and you need git to download it
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

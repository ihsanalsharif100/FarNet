# FarNet

this project lets users stream their windows device into a web browser

#how to compile

# 1.screemstream

1. download boost library

where?

from here {

https://www.boost.org/releases/latest/

needed:whatever your OS is.zip

how:download the zip and unzip it into C:\c++\lib\ or what you have in your os

}

or you could run in the cmd this{

run in cmd{

color 2(not optional)

git clone https://github.com/microsoft/vcpkg(download vcpkg and you need git)

cd vcpkg

.\bootstrap-vcpkg.bat

vcpkg install boost:x64-mingw-static

}

2.put this in Include directory:

C:\path\to\vcpkg\installed\x64-mingw-static\include

3.and this in the Library directory:

C:\path\to\vcpkg\installed\x64-mingw-static\lib

4.add this to the linker librares{

mswsock

ws2_32

gdi32

ole32

user32

gdiplus

}

or this in the other linker settings{

-lgdi32

-lmswsock

-lws2_32

-lole32

-luser32

-lgdiplus

}

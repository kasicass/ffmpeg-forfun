
:: set PATH & INCLUDE
set C99_DIR=%~dp0\c99\
set PATH=%C99_DIR%;%C99_DIR%c99toc89\;%PATH%
set INCLUDE=%C99_DIR%msinttypes\;%INCLUDE%

:: open msys.bat
D:\MinGW\msys\1.0\msys.bat

:: compile ffmpeg
:: ./configure toolchain=msvc
:: make
:: make install
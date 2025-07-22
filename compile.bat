@echo off
set /p version="Enter which version to compile (v1/v2/v3/v4): "
if not exist build mkdir build

echo ~~ FC-%version% Compiler ~~
if exist "src\%version%\win\encry-win.cpp" if exist "src\%version%\win\decry-win.cpp" (
    echo Compiling FC-%version%...
    g++ "src\%version%\win\encry-win.cpp" -o build\FC-%version% -std=c++11
    g++ "src\%version%\win\decry-win.cpp" -o build\FC-%version%_DECRYPTOR -std=c++11
    echo Finished compiling!
) else (
    echo Error: File not found! Please download the source code.
)

pause

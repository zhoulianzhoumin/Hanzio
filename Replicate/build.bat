@echo off
echo Compiling Hanzio...
g++ -std=c++11 -Wall -Wextra -o Hanzio.exe main.cpp
if %errorlevel% equ 0 (
    echo Compilation successful. Running...
    Hanzio.exe
) else (
    echo Compilation failed.
)
pause
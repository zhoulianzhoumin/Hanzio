@echo off
echo Compiling Hanzio OOP...
g++ -std=c++11 -Wall -Wextra -o Hanzio.exe main.cpp Coin.cpp Enemy.cpp GameObject.cpp Player.cpp QuestionBlock.cpp Renderer.cpp World.cpp
if %errorlevel% equ 0 (
    echo Compilation successful. Running...
    Hanzio.exe
) else (
    echo Compilation failed.
)
pause
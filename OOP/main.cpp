#include <windows.h>
#include <cstdio>
#include "Renderer.hpp"
#include "World.hpp"
#include "Constants.hpp"

int main() {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hIn, &mode);
    SetConsoleMode(hIn, (mode & ~(ENABLE_QUICK_EDIT_MODE | ENABLE_MOUSE_INPUT)) | ENABLE_EXTENDED_FLAGS);

    Renderer renderer;
    World world;
	//world.LoadLevel(2);

    while (1) {
        DWORD tmpMode;
        GetConsoleMode(hIn, &tmpMode);
        SetConsoleMode(hIn, (tmpMode & ~(ENABLE_QUICK_EDIT_MODE | ENABLE_MOUSE_INPUT)) | ENABLE_EXTENDED_FLAGS);

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
        if (GetAsyncKeyState('A') & 0x8000) world.MovePlayer(-MOVE_SPEED);
		if (GetAsyncKeyState('D') & 0x8000) world.MovePlayer(MOVE_SPEED);
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) world.GetPlayer().Jump();

        world.Update();

        renderer.Clear();
        world.Draw(renderer);
        renderer.Present((int)world.GetPlayer().GetX());

        Sleep(FRAME_DELAY);
    }
    return 0;
}
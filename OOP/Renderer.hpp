#ifndef RENDERER_H
#define RENDERER_H

#include <cstdio>
#include <cstring>
#include <cmath>
#include <windows.h>

class Renderer {
private:
    char buffer[25][257];  // mapHeight=25, mapWidth=256+1

    char GetDisplayChar(wchar_t wch);

public:
    void Clear();
    void DrawChar(int x, int y, wchar_t symbol);
    void DrawObject(float x, float y, float width, float height, wchar_t symbol);
    void DrawScore(int score);
    void Present(int playerX);
    void SetCursor(int x, int y);
};

#endif
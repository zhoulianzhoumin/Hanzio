#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Constants.hpp"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <windows.h>

class Renderer {
private:
    char buffer[MAP_HEIGHT][MAP_WIDTH + 1];
    HANDLE hOutput;
public:
    Renderer();
    void Clear();
    void DrawChar(int x, int y, char c);
    void DrawRect(float x, float y, float w, float h, char c);
    void DrawScore(int score);
    void Present(int playerX);
};

#endif
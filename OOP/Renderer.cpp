#include "Renderer.h"

char Renderer::GetDisplayChar(wchar_t wch) {
    switch (wch) {
    case L'我': return '@';
    case L'秘': return '?';
    case L'已': return '-';
    case L'木': return '#';
    case L'星': return '+';
    case L'危': return 'o';
    case L'元': return '$';
    default: return '?';
    }
}

void Renderer::Clear() {
    for (int i = 0; i < 256; i++)
        buffer[0][i] = ' ';
    buffer[0][256] = '\0';
    for (int j = 1; j < 25; j++)
        std::sprintf(buffer[j], "%s", buffer[0]);
}

void Renderer::DrawChar(int x, int y, wchar_t symbol) {
    if (x >= 0 && x < 256 && y >= 0 && y < 25)
        buffer[y][x] = GetDisplayChar(symbol);
}

void Renderer::DrawObject(float x, float y, float width, float height, wchar_t symbol) {
    int ix = (int)round(x);
    int iy = (int)round(y);
    int iw = (int)round(width);
    int ih = (int)round(height);
    for (int i = ix; i < ix + iw; i++)
        for (int j = iy; j < iy + ih; j++)
            if (i >= 0 && i < 256 && j >= 0 && j < 25)
                buffer[j][i] = GetDisplayChar(symbol);
}

void Renderer::DrawScore(int score) {
    char c[30];
    sprintf(c, "Score:%d", score);
    int len = strlen(c);
    for (int i = 0; i < len && i + 5 < 256; i++)
        buffer[1][i + 5] = c[i];
}

void Renderer::Present(int playerX) {
    const int viewWidth = 80;
    int startCol = (int)round(playerX) - viewWidth / 2;
    if (startCol < 0) startCol = 0;
    if (startCol + viewWidth > 256) startCol = 256 - viewWidth;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int j = 0; j < 25; j++) {
        buffer[j][startCol + viewWidth] = '\0';
        printf("%s\n", buffer[j] + startCol);
        buffer[j][startCol + viewWidth] = ' ';
    }
}

void Renderer::SetCursor(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
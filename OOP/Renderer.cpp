#include "Renderer.hpp"

Renderer::Renderer() {
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufSize = { MAP_WIDTH + 1, MAP_HEIGHT + 1 };
    SetConsoleScreenBufferSize(hOutput, bufSize);
    SMALL_RECT windowRect = { 0, 0, VIEW_WIDTH - 1, VIEW_HEIGHT - 1 };
    SetConsoleWindowInfo(hOutput, TRUE, &windowRect);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOutput, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOutput, &cursorInfo);
}

void Renderer::Clear() {
    for (int j = 0; j < MAP_HEIGHT; j++) {
        for (int i = 0; i < MAP_WIDTH; i++)
            buffer[j][i] = ' ';
        buffer[j][MAP_WIDTH] = '\0';
    }
}

void Renderer::DrawChar(int x, int y, char c) {
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)
        buffer[y][x] = c;
}

void Renderer::DrawRect(float x, float y, float w, float h, char c) {
    int x0 = (int)round(x);
    int y0 = (int)round(y);
    int x1 = (int)round(x + w);
    int y1 = (int)round(y + h);
    for (int i = x0; i < x1; i++)
        for (int j = y0; j < y1; j++)
            DrawChar(i, j, c);
}

void Renderer::DrawScore(int score) {
    char scoreStr[30];
    sprintf(scoreStr, "Score: %d", score);
    int len = strlen(scoreStr);
    for (int i = 0; i < len && i + 2 < MAP_WIDTH; i++)
        buffer[0][i + 2] = scoreStr[i];
}

void Renderer::Present(int playerX) {
    COORD origin = { 0, 0 };
    SetConsoleCursorPosition(hOutput, origin);

    // 预计算总字符数，拼成一个大字符串
    int totalLen = MAP_HEIGHT * (MAP_WIDTH + 1); // 每行 MAP_WIDTH 字符 + 换行
    char* frame = new char[totalLen + 1];

    int pos = 0;
    // 第 0 行（分数行）完整输出
    for (int i = 0; i < MAP_WIDTH; i++)
        frame[pos++] = buffer[0][i];
    frame[pos++] = '\n';

    // 视口裁剪
    int startCol = playerX - VIEW_WIDTH / 2;
    if (startCol < 0) startCol = 0;
    if (startCol + VIEW_WIDTH > MAP_WIDTH) startCol = MAP_WIDTH - VIEW_WIDTH;

    for (int j = 1; j < MAP_HEIGHT; j++) {
        for (int i = startCol; i < startCol + VIEW_WIDTH; i++)
            frame[pos++] = buffer[j][i];
        frame[pos++] = '\n';
    }
    frame[pos] = '\0';

    printf("%s", frame);
    delete[] frame;
}
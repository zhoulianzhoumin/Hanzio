#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <windows.h>

// ========== 游戏物理参数 ==========
const float GRAVITY              = 0.08f;
const float JUMP_SPEED           = -1.2f;
const float COIN_BOUNCE_SPEED    = -0.7f;
const float ENEMY_HORIZ_SPEED    = 0.2f;

// ========== 显示参数 ==========
const int VIEW_WIDTH  = 80;
const int VIEW_HEIGHT = 25;

// ========== 时间参数（毫秒） ==========
const int FRAME_DELAY          = 10;
const int DEATH_DELAY          = 1000;
const int LEVEL_COMPLETE_DELAY = 500;

// ========== 分数参数 ==========
const int SCORE_STOMP = 50;
const int SCORE_COIN  = 100;

#define mapWidth 256
#define mapHeight 25

typedef int BOOL;
#define TRUE 1
#define FALSE 0

typedef struct SObject {
    float x, y;
    float width, height;
    float vertSpeed;
    BOOL IsFly;
    wchar_t cType;          // 内部存储汉字
    float horizSpeed;
} TObject;

char map[mapHeight][mapWidth + 1];
TObject Hanzio;
TObject *brick = NULL;
int brickLength;
TObject *moving = NULL;
int movingLength;
int level = 1;
int score;
int maxLvl;

// 函数声明
void ClearMap();
void ShowMap();
void setCur(int x, int y);
void SetObjectPos(TObject *obj, float xPos, float yPos);
void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, wchar_t inType);
BOOL IsCollision(TObject o1, TObject o2);
void CreateLevel(int lvl);
TObject *GetNewMoving();
void VertMoveObject(TObject *obj);
void DeleteMoving(int i);
void HanzioCollision();
void HorizonMoveObject(TObject *obj);
BOOL IsPosInMap(int x, int y);
char GetDisplayChar(wchar_t wch);
void PutObjectOnMap(TObject obj);
void HorizonMoveMap(float dx);
TObject *GetNewBrick();
void PutScoreOnMap();

// 实现
void ClearMap() {
    for (int i = 0; i < mapWidth; i++)
        map[0][i] = ' ';
    map[0][mapWidth] = '\0';
    for (int j = 1; j < mapHeight; j++)
        std::sprintf(map[j], "%s", map[0]);
}

void setCur(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void ShowMap() {
    const int viewWidth = VIEW_WIDTH;
    int playerCol = (int)round(Hanzio.x);
    int startCol = playerCol - viewWidth / 2;
    if (startCol < 0) startCol = 0;
    if (startCol + viewWidth > mapWidth) startCol = mapWidth - viewWidth;
    for (int j = 0; j < mapHeight; j++) {
        map[j][startCol + viewWidth] = '\0';
        printf("%s\n", map[j] + startCol);
        map[j][startCol + viewWidth] = ' ';
    }
}

void SetObjectPos(TObject *obj, float xPos, float yPos) {
    obj->x = xPos;
    obj->y = yPos;
}

void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, wchar_t inType) {
    SetObjectPos(obj, xPos, yPos);
    obj->width = oWidth;
    obj->height = oHeight;
    obj->vertSpeed = 0;
    obj->cType = inType;
    obj->horizSpeed = ENEMY_HORIZ_SPEED;
    obj->IsFly = FALSE;
}

BOOL IsCollision(TObject o1, TObject o2) {
    return (o1.x + o1.width) > o2.x &&
           o1.x < (o2.x + o2.width) &&
           (o1.y + o1.height) > o2.y &&
           o1.y < (o2.y + o2.height);
}

void PlayerDead() {
    system("color 4F");
    Sleep(DEATH_DELAY);
    CreateLevel(level);
}

void VertMoveObject(TObject *obj) {
    obj->IsFly = TRUE;
    obj->vertSpeed += GRAVITY;
    SetObjectPos(obj, obj->x, obj->y + obj->vertSpeed);
    for (int i = 0; i < brickLength; i++) {
        if (IsCollision(*obj, brick[i])) {
            if ((brick[i].cType == L'秘') && (obj->vertSpeed < 0) && (obj == &Hanzio)) {
                brick[i].cType = L'已';
                InitObject(GetNewMoving(), brick[i].x, brick[i].y - 3, 3, 2, L'元');
                moving[movingLength - 1].vertSpeed = COIN_BOUNCE_SPEED;
            }
            obj->y -= obj->vertSpeed;
            obj->vertSpeed = 0;
            obj->IsFly = FALSE;
            if (brick[i].cType == L'星') {
                level++;
                if (level > maxLvl) level = 1;
                system("color 2F");
                Sleep(LEVEL_COMPLETE_DELAY);
                CreateLevel(level);
            }
            break;
        }
    }
}

void DeleteMoving(int i) {
    movingLength--;
    moving[i] = moving[movingLength];
    moving = (TObject*)realloc(moving, sizeof(*moving) * movingLength);
}

void HanzioCollision() {
    for (int i = 0; i < movingLength; i++) {
        if (IsCollision(Hanzio, moving[i])) {
            if (moving[i].cType == L'危') {
                if (Hanzio.IsFly == TRUE && Hanzio.vertSpeed > 0 &&
                    Hanzio.y + Hanzio.height < moving[i].y + moving[i].height * 0.5f) {
                    score += SCORE_STOMP;
                    DeleteMoving(i);
                    i--;
                    continue;
                } else {
                    PlayerDead();
                }
            }
            if (moving[i].cType == L'元') {
                score += SCORE_COIN;
                DeleteMoving(i);
                i--;
                continue;
            }
        }
    }
}

void HorizonMoveObject(TObject *obj) {
    obj->x += obj->horizSpeed;
    for (int i = 0; i < brickLength; i++) {
        if (IsCollision(*obj, brick[i])) {
            obj->x -= obj->horizSpeed;
            obj->horizSpeed = -obj->horizSpeed;
            return;
        }
    }
    if (obj->cType == L'危') {
        TObject tmp = *obj;
        VertMoveObject(&tmp);
        if (tmp.IsFly == TRUE) {
            obj->x -= obj->horizSpeed;
            obj->horizSpeed = -obj->horizSpeed;
        }
    }
}

BOOL IsPosInMap(int x, int y) {
    return (x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight);
}

char GetDisplayChar(wchar_t wch) {
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

void PutObjectOnMap(TObject obj) {
    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidth = (int)round(obj.width);
    int iHeight = (int)round(obj.height);
    for (int i = ix; i < (ix + iWidth); i++)
        for (int j = iy; j < (iy + iHeight); j++)
            if (IsPosInMap(i, j))
                map[j][i] = GetDisplayChar(obj.cType);
}

void HorizonMoveMap(float dx) {
    Hanzio.x -= dx;
    for (int i = 0; i < brickLength; i++)
        if (IsCollision(Hanzio, brick[i])) {
            Hanzio.x += dx;
            return;
        }
    Hanzio.x += dx;
    for (int i = 0; i < brickLength; i++)
        brick[i].x += dx;
    for (int i = 0; i < movingLength; i++)
        moving[i].x += dx;
}

TObject *GetNewBrick() {
    brickLength++;
    brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength);
    return brick + brickLength - 1;
}

TObject *GetNewMoving() {
    movingLength++;
    moving = (TObject*)realloc(moving, sizeof(*moving) * movingLength);
    return moving + movingLength - 1;
}

void PutScoreOnMap() {
    char c[30];
    sprintf(c, "Score:%d", score);
    int len = strlen(c);
    for (int i = 0; i < len; i++)
        map[1][i + 5] = c[i];
}

void CreateLevel(int lvl) {
    system("color 9F");
    free(brick);
    brick = NULL;
    brickLength = 0;
    free(moving);
    moving = NULL;
    movingLength = 0;
    InitObject(&Hanzio, 10, 15, 3, 3, L'我');
    score = 0;

    if (lvl == 1) {
        InitObject(GetNewBrick(), 0, 20, 200, 5, L'木');
        InitObject(GetNewBrick(), 30, 15, 35, 3, L'木');
        InitObject(GetNewBrick(), 25, 6, 6, 4, L'秘');
        InitObject(GetNewBrick(), 40, 5, 6, 4, L'秘');
        InitObject(GetNewBrick(), 55, 4, 6, 4, L'秘');
        InitObject(GetNewBrick(), 70, 3, 6, 4, L'秘');
        InitObject(GetNewBrick(), 75, 12, 25, 2, L'木');
        InitObject(GetNewBrick(), 80, 20, 20, 5, L'木');
        InitObject(GetNewBrick(), 120, 15, 10, 10, L'木');
        InitObject(GetNewBrick(), 100, 12, 35, 2, L'木');
        InitObject(GetNewBrick(), 97, 8, 5, 4, L'木');
        InitObject(GetNewBrick(), 99, 0, 4, 3, L'秘');
        InitObject(GetNewBrick(), 114, 1, 4, 3, L'秘');
        InitObject(GetNewBrick(), 110, 9, 5, 2, L'木');
        InitObject(GetNewBrick(), 115, 14, 7, 5, L'木');
        InitObject(GetNewBrick(), 190, 17, 15, 2, L'木');
        InitObject(GetNewBrick(), 210, 15, 10, 10, L'星');
    }
    else if (lvl == 2) {
        InitObject(GetNewBrick(), 0, 20, 60, 5, L'木');
        InitObject(GetNewBrick(), 80, 20, 30, 5, L'木');
        InitObject(GetNewBrick(), 150, 20, 60, 5, L'木');
        InitObject(GetNewBrick(), 20, 14, 25, 2, L'木');
        InitObject(GetNewBrick(), 25, 2, 5, 4, L'秘');
        InitObject(GetNewBrick(), 30, 2, 5, 4, L'秘');
        InitObject(GetNewBrick(), 35, 2, 5, 4, L'秘');
        InitObject(GetNewBrick(), 40, 2, 5, 4, L'秘');
        InitObject(GetNewMoving(), 22, 9, 3, 2, L'危');
        InitObject(GetNewMoving(), 38, 9, 3, 2, L'危');
        InitObject(GetNewBrick(), 60, 15, 10, 10, L'木');
        InitObject(GetNewBrick(), 120, 15, 10, 10, L'木');
        InitObject(GetNewBrick(), 85, 12, 10, 2, L'木');
        InitObject(GetNewBrick(), 97, 9, 10, 2, L'木');
        InitObject(GetNewBrick(), 114, 8, 5, 2, L'木');
        InitObject(GetNewBrick(), 105, 0, 5, 3, L'秘');
        InitObject(GetNewBrick(), 112, 0, 5, 3, L'秘');
        InitObject(GetNewMoving(), 93, 10, 3, 2, L'危');
        InitObject(GetNewMoving(), 100, 8, 3, 2, L'危');
        InitObject(GetNewMoving(), 112, 6, 3, 2, L'危');
        InitObject(GetNewMoving(), 70, 13, 3, 2, L'危');
        InitObject(GetNewMoving(), 130, 13, 3, 2, L'危');
        InitObject(GetNewMoving(), 25, 19, 3, 2, L'危');
        InitObject(GetNewMoving(), 50, 19, 3, 2, L'危');
        InitObject(GetNewMoving(), 90, 19, 3, 2, L'危');
        InitObject(GetNewMoving(), 160, 19, 3, 2, L'危');
        InitObject(GetNewMoving(), 180, 19, 3, 2, L'危');
        InitObject(GetNewBrick(), 190, 17, 15, 2, L'木');
        InitObject(GetNewBrick(), 210, 15, 10, 10, L'星');
    }
    else if (lvl == 3) {
        InitObject(GetNewBrick(), 0, 20, 25, 5, L'木');
        InitObject(GetNewBrick(), 35, 20, 25, 5, L'木');
        InitObject(GetNewBrick(), 28, 24, 10, 1, L'木');
        InitObject(GetNewBrick(), 70, 20, 50, 5, L'木');
        InitObject(GetNewBrick(), 170, 20, 45, 5, L'木');
        InitObject(GetNewBrick(), 40, 13, 5, 12, L'木');
        InitObject(GetNewBrick(), 75, 13, 5, 12, L'木');
        InitObject(GetNewBrick(), 45, 16, 30, 2, L'木');
        InitObject(GetNewBrick(), 55, 10, 10, 2, L'秘');
        InitObject(GetNewMoving(), 50, 14, 3, 2, L'危');
        InitObject(GetNewMoving(), 65, 14, 3, 2, L'危');
        InitObject(GetNewBrick(), 85, 12, 15, 1, L'木');
        InitObject(GetNewBrick(), 100, 14, 50, 2, L'木');
        InitObject(GetNewBrick(), 100, 0, 18, 2, L'木');
        InitObject(GetNewBrick(), 130, 0, 18, 2, L'木');
        InitObject(GetNewBrick(), 100, 6, 2, 2, L'木');
        InitObject(GetNewBrick(), 100, 12, 2, 2, L'木');
        InitObject(GetNewBrick(), 148, 6, 2, 2, L'木');
        InitObject(GetNewBrick(), 148, 12, 2, 2, L'木');
        InitObject(GetNewBrick(), 110, 13, 10, 1, L'木');
        InitObject(GetNewBrick(), 130, 13, 10, 1, L'木');
        InitObject(GetNewBrick(), 110, 7, 30, 1, L'木');
        InitObject(GetNewBrick(), 122, 8, 6, 2, L'秘');
        InitObject(GetNewMoving(), 118, 11, 3, 2, L'危');
        InitObject(GetNewMoving(), 130, 11, 3, 2, L'危');
        InitObject(GetNewBrick(), 104, 6, 4, 2, L'秘');
        InitObject(GetNewBrick(), 142, 6, 4, 2, L'秘');
        InitObject(GetNewMoving(), 104, 10, 3, 2, L'危');
        InitObject(GetNewMoving(), 141, 12, 3, 2, L'危');
        InitObject(GetNewBrick(), 121, 1, 8, 2, L'秘');
        InitObject(GetNewBrick(), 130, 24, 40, 2, L'木');
        InitObject(GetNewMoving(), 138, 22, 3, 2, L'危');
        InitObject(GetNewMoving(), 152, 22, 3, 2, L'危');
        InitObject(GetNewBrick(), 140, 17, 5, 3, L'秘');
        InitObject(GetNewBrick(), 155, 16, 5, 3, L'秘');
        InitObject(GetNewBrick(), 200, 17, 15, 2, L'木');
        InitObject(GetNewBrick(), 215, 15, 10, 10, L'星');
    }
    maxLvl = 3;
}

int main() {
    system("chcp 437 > nul");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = { mapWidth + 1, mapHeight + 1 };
    SetConsoleScreenBufferSize(hConsole, bufferSize);
    SMALL_RECT windowRect = { 0, 0, VIEW_WIDTH - 1, VIEW_HEIGHT - 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowRect);
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode(hInput, &consoleMode);
    consoleMode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_MOUSE_INPUT);
    SetConsoleMode(hInput, consoleMode);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    CreateLevel(level);

    do {
        ClearMap();
        if ((Hanzio.IsFly == FALSE) && (GetAsyncKeyState(VK_SPACE) & 0x8000))
            Hanzio.vertSpeed = JUMP_SPEED;
        if (GetAsyncKeyState('A') & 0x8000) HorizonMoveMap(1);
        if (GetAsyncKeyState('D') & 0x8000) HorizonMoveMap(-1);
        VertMoveObject(&Hanzio);
        if (Hanzio.y > mapHeight) PlayerDead();
        HanzioCollision();
        for (int i = 0; i < brickLength; i++)
            PutObjectOnMap(brick[i]);
        for (int i = 0; i < movingLength; i++) {
            VertMoveObject(moving + i);
            HorizonMoveObject(moving + i);
            if (moving[i].y > mapHeight) {
                DeleteMoving(i);
                i--;
                continue;
            }
            PutObjectOnMap(moving[i]);
        }
        PutObjectOnMap(Hanzio);
        PutScoreOnMap();
        setCur(0, 0);
        ShowMap();
        Sleep(FRAME_DELAY);
    } while (GetAsyncKeyState(VK_ESCAPE) >= 0);
    return 0;
}
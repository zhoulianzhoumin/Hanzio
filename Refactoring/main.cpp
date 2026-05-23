#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <windows.h>

// ================== 常数列 =================
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

// ================ 函数声明 ================
// 渲染系统
void setCur(int x, int y);
void ClearMap();
void ShowMap();
void PutObjectOnMap(TObject obj);
void PutScoreOnMap();
char GetDisplayChar(wchar_t wch);

// 工具函数
void SetObjectPos(TObject *obj, float xPos, float yPos);
void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, wchar_t inType);
void DeleteMoving(int i);
BOOL IsPosInMap(int x, int y);
BOOL IsCollision(TObject o1, TObject o2);
TObject *GetNewBrick();
TObject *GetNewMoving();

// 物理系统
void VertMoveObject(TObject *obj);

// 敌人系统
void HorizonMoveObject(TObject *obj);

// 碰撞系统
void HanzioCollision();

// 卷轴系统
void HorizonMoveMap(float dx);

//关卡系统
void CreateLevel(int lvl);

// 游戏流程
void PlayerDead();

// ========== 拆分出的辅助函数声明 ==========
// 物理系统辅助
void HandleQuestionBlock(int brickIndex);
void HandleLanding(TObject *obj);
void HandleLevelComplete(int brickIndex);

// 碰撞系统辅助
void HandleEnemyCollision(int i);
void HandleCoinCollision(int i);

// 关卡加载
void LoadLevel1();
void LoadLevel2();
void LoadLevel3();

// 主循环拆分
void InitConsole();
void ProcessInput();
void UpdateWorld();
void RenderFrame();

// 实现
// 渲染系统
void setCur(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void ClearMap() {
    for (int i = 0; i < mapWidth; i++)
        map[0][i] = ' ';
    map[0][mapWidth] = '\0';
    for (int j = 1; j < mapHeight; j++)
        std::sprintf(map[j], "%s", map[0]);
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

void PutScoreOnMap() {
    char c[30];
    sprintf(c, "Score:%d", score);
    int len = strlen(c);
    for (int i = 0; i < len; i++)
        map[1][i + 5] = c[i];
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

// 工具函数
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

void DeleteMoving(int i) {
    movingLength--;
    moving[i] = moving[movingLength];
    moving = (TObject*)realloc(moving, sizeof(*moving) * movingLength);
}

BOOL IsPosInMap(int x, int y) {
    return (x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight);
}

BOOL IsCollision(TObject o1, TObject o2) {
    return (o1.x + o1.width) > o2.x &&
           o1.x < (o2.x + o2.width) &&
           (o1.y + o1.height) > o2.y &&
           o1.y < (o2.y + o2.height);
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

// 物理系统
// ========== 处理问号砖（玩家从下方顶） ==========
void HandleQuestionBlock(int brickIndex)
{
    if ((brick[brickIndex].cType == L'秘') &&
        (Hanzio.vertSpeed < 0))           // 玩家必须向上顶
    {
        brick[brickIndex].cType = L'已';   // 问号砖变已顶砖
        InitObject(GetNewMoving(),
            brick[brickIndex].x,
            brick[brickIndex].y - 3,
            3, 2, L'元');                  // 上方生成金币
        moving[movingLength - 1].vertSpeed = COIN_BOUNCE_SPEED;  // 金币弹跳
    }
}

// ========== 处理落地（回退 Y，停止速度） ==========
void HandleLanding(TObject *obj)
{
    obj->y -= obj->vertSpeed;              // 撤回本帧下落
    obj->vertSpeed = 0;                    // 垂直速度归零
    obj->IsFly = FALSE;                    // 标记落地
}

// ========== 处理过关砖块 ==========
void HandleLevelComplete(int brickIndex)
{
    if (brick[brickIndex].cType == L'星')
    {
        level++;
        if (level > maxLvl) level = 1;
        system("color 2F");                // 绿屏过关特效
        Sleep(LEVEL_COMPLETE_DELAY);
        CreateLevel(level);                // 加载下一关
    }
}

void VertMoveObject(TObject *obj)
{
    obj->IsFly = TRUE;                     // 1. 假设在空中
    obj->vertSpeed += GRAVITY;             // 2. 重力加速
    SetObjectPos(obj, obj->x, obj->y + obj->vertSpeed);  // 3. 更新位置

    for (int i = 0; i < brickLength; i++)
    {
        if (IsCollision(*obj, brick[i]))
        {
            if (obj == &Hanzio)            // 只有玩家能触发问号砖
                HandleQuestionBlock(i);
            HandleLanding(obj);
            HandleLevelComplete(i);
            break;
        }
    }
}

// 敌人系统
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

// 碰撞系统
// ========== 处理玩家与敌人的碰撞 ==========
void HandleEnemyCollision(int i)
{
    if (moving[i].cType != L'危') return;

    if ((Hanzio.IsFly == TRUE) &&
        (Hanzio.vertSpeed > 0) &&                              // 玩家在下落
        (Hanzio.y + Hanzio.height < moving[i].y + moving[i].height * 0.5f))  // 踩到头顶
    {
        score += SCORE_STOMP;                                   // 踩死敌人得分
        DeleteMoving(i);
    }
    else
    {
        PlayerDead();                                           // 否则玩家死亡
    }
}

// ========== 处理玩家与金币的碰撞 ==========
void HandleCoinCollision(int i)
{
    if (moving[i].cType != L'元') return;

    score += SCORE_COIN;                                        // 吃金币得分
    DeleteMoving(i);
}

void HanzioCollision()
{
    for (int i = 0; i < movingLength; i++)
    {
        if (IsCollision(Hanzio, moving[i]))
        {
            int oldLength = movingLength;                       // 记录删除前的长度
            HandleEnemyCollision(i);
            if (movingLength < oldLength) { i--; continue; }    // 敌人被删，回退索引

            HandleCoinCollision(i);
            if (movingLength < oldLength) { i--; continue; }    // 金币被删，回退索引
        }
    }
}

// 卷轴系统
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

//关卡系统
// ========== 加载第一关 ==========
void LoadLevel1()
{
    // 地面
    InitObject(GetNewBrick(), 0, 20, 200, 5, L'木');

    // 第一金币阵垫脚平台
    InitObject(GetNewBrick(), 30, 15, 35, 3, L'木');

    // 第一金币阵
    InitObject(GetNewBrick(), 25, 6, 6, 4, L'秘');
    InitObject(GetNewBrick(), 40, 5, 6, 4, L'秘');
    InitObject(GetNewBrick(), 55, 4, 6, 4, L'秘');
    InitObject(GetNewBrick(), 70, 3, 6, 4, L'秘');

    // 衔接平台
    InitObject(GetNewBrick(), 75, 12, 25, 2, L'木');

    // 中段障碍
    InitObject(GetNewBrick(), 80, 20, 20, 5, L'木');
    InitObject(GetNewBrick(), 120, 15, 10, 10, L'木');

    // 第二金币阵
    InitObject(GetNewBrick(), 100, 12, 35, 2, L'木');
    InitObject(GetNewBrick(), 97, 8, 5, 4, L'木');
    InitObject(GetNewBrick(), 99, 0, 4, 3, L'秘');
    InitObject(GetNewBrick(), 114, 1, 4, 3, L'秘');

    // 浮空台阶
    InitObject(GetNewBrick(), 110, 9, 5, 2, L'木');

    // 长平台
    InitObject(GetNewBrick(), 115, 14, 7, 5, L'木');

    // 过关助跑平台
    InitObject(GetNewBrick(), 190, 17, 15, 2, L'木');

    // 过关砖块
    InitObject(GetNewBrick(), 210, 15, 10, 10, L'星');
}

// ========== 加载第二关 ==========
void LoadLevel2()
{
    // 分段地面（死亡坑）
    InitObject(GetNewBrick(), 0, 20, 60, 5, L'木');
    InitObject(GetNewBrick(), 80, 20, 30, 5, L'木');
    InitObject(GetNewBrick(), 150, 20, 60, 5, L'木');

    // 第一金币阵
    InitObject(GetNewBrick(), 20, 14, 25, 2, L'木');
    InitObject(GetNewBrick(), 25, 2, 5, 4, L'秘');
    InitObject(GetNewBrick(), 30, 2, 5, 4, L'秘');
    InitObject(GetNewBrick(), 35, 2, 5, 4, L'秘');
    InitObject(GetNewBrick(), 40, 2, 5, 4, L'秘');

    // 活台阶敌人
    InitObject(GetNewMoving(), 22, 9, 3, 2, L'危');
    InitObject(GetNewMoving(), 38, 9, 3, 2, L'危');

    // 中段障碍
    InitObject(GetNewBrick(), 60, 15, 10, 10, L'木');
    InitObject(GetNewBrick(), 120, 15, 10, 10, L'木');

    // 第二金币阵
    InitObject(GetNewBrick(), 85, 12, 10, 2, L'木');
    InitObject(GetNewBrick(), 97, 9, 10, 2, L'木');
    InitObject(GetNewBrick(), 114, 8, 5, 2, L'木');
    InitObject(GetNewBrick(), 105, 0, 5, 3, L'秘');
    InitObject(GetNewBrick(), 112, 0, 5, 3, L'秘');

    // 台阶敌人
    InitObject(GetNewMoving(), 93, 10, 3, 2, L'危');
    InitObject(GetNewMoving(), 100, 8, 3, 2, L'危');
    InitObject(GetNewMoving(), 112, 6, 3, 2, L'危');

    // 坑上方浮空敌人
    InitObject(GetNewMoving(), 70, 13, 3, 2, L'危');
    InitObject(GetNewMoving(), 130, 13, 3, 2, L'危');

    // 地面巡逻敌人
    InitObject(GetNewMoving(), 25, 19, 3, 2, L'危');
    InitObject(GetNewMoving(), 50, 19, 3, 2, L'危');
    InitObject(GetNewMoving(), 90, 19, 3, 2, L'危');
    InitObject(GetNewMoving(), 160, 19, 3, 2, L'危');
    InitObject(GetNewMoving(), 180, 19, 3, 2, L'危');

    // 过关助跑平台
    InitObject(GetNewBrick(), 190, 17, 15, 2, L'木');

    // 过关砖块
    InitObject(GetNewBrick(), 210, 15, 10, 10, L'星');
}

// ========== 加载第三关 ==========
void LoadLevel3()
{
    // 地面
    InitObject(GetNewBrick(), 0, 20, 25, 5, L'木');
    InitObject(GetNewBrick(), 35, 20, 25, 5, L'木');
    InitObject(GetNewBrick(), 28, 24, 10, 1, L'木');
    InitObject(GetNewBrick(), 70, 20, 50, 5, L'木');
    InitObject(GetNewBrick(), 170, 20, 45, 5, L'木');

    // 凹字形楼阁
    InitObject(GetNewBrick(), 40, 13, 5, 12, L'木');
    InitObject(GetNewBrick(), 75, 13, 5, 12, L'木');
    InitObject(GetNewBrick(), 45, 16, 30, 2, L'木');
    InitObject(GetNewBrick(), 55, 10, 10, 2, L'秘');
    InitObject(GetNewMoving(), 50, 14, 3, 2, L'危');
    InitObject(GetNewMoving(), 65, 14, 3, 2, L'危');

    // 过渡浮桥
    InitObject(GetNewBrick(), 85, 12, 15, 1, L'木');

    // 回字形迷宫外圈
    InitObject(GetNewBrick(), 100, 14, 50, 2, L'木');
    InitObject(GetNewBrick(), 100, 0, 18, 2, L'木');
    InitObject(GetNewBrick(), 130, 0, 18, 2, L'木');
    InitObject(GetNewBrick(), 100, 6, 2, 2, L'木');
    InitObject(GetNewBrick(), 100, 12, 2, 2, L'木');
    InitObject(GetNewBrick(), 148, 6, 2, 2, L'木');
    InitObject(GetNewBrick(), 148, 12, 2, 2, L'木');

    // 回字形迷宫内圈
    InitObject(GetNewBrick(), 110, 13, 10, 1, L'木');
    InitObject(GetNewBrick(), 130, 13, 10, 1, L'木');
    InitObject(GetNewBrick(), 110, 7, 30, 1, L'木');

    // 中央大型问号砖
    InitObject(GetNewBrick(), 122, 8, 6, 2, L'秘');

    // 巡逻敌人
    InitObject(GetNewMoving(), 118, 11, 3, 2, L'危');
    InitObject(GetNewMoving(), 130, 11, 3, 2, L'危');

    // 外圈问号砖
    InitObject(GetNewBrick(), 104, 6, 4, 2, L'秘');
    InitObject(GetNewBrick(), 142, 6, 4, 2, L'秘');

    // 外圈敌人
    InitObject(GetNewMoving(), 104, 10, 3, 2, L'危');
    InitObject(GetNewMoving(), 141, 12, 3, 2, L'危');

    // 外圈顶下奖励
    InitObject(GetNewBrick(), 121, 1, 8, 2, L'秘');

    // 大坑金币阵
    InitObject(GetNewBrick(), 130, 24, 40, 2, L'木');
    InitObject(GetNewMoving(), 138, 22, 3, 2, L'危');
    InitObject(GetNewMoving(), 152, 22, 3, 2, L'危');
    InitObject(GetNewBrick(), 140, 17, 5, 3, L'秘');
    InitObject(GetNewBrick(), 155, 16, 5, 3, L'秘');

    // 过关
    InitObject(GetNewBrick(), 200, 17, 15, 2, L'木');
    InitObject(GetNewBrick(), 215, 15, 10, 10, L'星');
}

void CreateLevel(int lvl)
{
    system("color 9F");

    free(brick);
    brick = NULL;
    brickLength = 0;

    free(moving);
    moving = NULL;
    movingLength = 0;

    InitObject(&Hanzio, 10, 15, 3, 3, L'我');
    score = 0;

    if (lvl == 1)      LoadLevel1();
    else if (lvl == 2) LoadLevel2();
    else if (lvl == 3) LoadLevel3();

    maxLvl = 3;
}

// 游戏流程
void PlayerDead() {
    system("color 4F");
    Sleep(DEATH_DELAY);
    CreateLevel(level);
}

// ========== 控制台初始化 ==========
void InitConsole()
{
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
}

// ========== 处理玩家输入 ==========
void ProcessInput()
{
    if ((Hanzio.IsFly == FALSE) && (GetAsyncKeyState(VK_SPACE) & 0x8000))
        Hanzio.vertSpeed = JUMP_SPEED;
    if (GetAsyncKeyState('A') & 0x8000) HorizonMoveMap(1);
    if (GetAsyncKeyState('D') & 0x8000) HorizonMoveMap(-1);
}

// ========== 更新游戏世界（一帧的逻辑） ==========
void UpdateWorld()
{
    VertMoveObject(&Hanzio);
    if (Hanzio.y > mapHeight) PlayerDead();
    HanzioCollision();

    for (int i = 0; i < movingLength; i++)
    {
        VertMoveObject(moving + i);
        HorizonMoveObject(moving + i);
        if (moving[i].y > mapHeight)
        {
            DeleteMoving(i);
            i--;
            continue;
        }
    }
}

// ========== 渲染一帧画面 ==========
void RenderFrame()
{
    ClearMap();

    for (int i = 0; i < brickLength; i++)
        PutObjectOnMap(brick[i]);
    for (int i = 0; i < movingLength; i++)
        PutObjectOnMap(moving[i]);

    PutObjectOnMap(Hanzio);
    PutScoreOnMap();

    setCur(0, 0);
    ShowMap();
}

int main()
{
    InitConsole();
    CreateLevel(level);

    do
    {
        ProcessInput();
        UpdateWorld();
        RenderFrame();
        Sleep(FRAME_DELAY);
    }
    while (GetAsyncKeyState(VK_ESCAPE) >= 0);

    return 0;
}
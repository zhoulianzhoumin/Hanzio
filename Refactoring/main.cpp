#include <cstdio> //打印函数
#include <cstdlib> //system, realloc（重新分配动态内存）
#include <cstring>   // memset 需要这个头文件

#include <cmath>
#include <windows.h>
//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE)，获取标准输入、输出或错误设备的“句柄”（操作凭证）
//SetConsoleScreenBufferSize(hConsole, bufferSize); 定义控制台内部缓冲区大小（字符网格的“画布”大小）。如果地图大于默认缓冲区，地图就会被截断。把它设为 mapWidth+1 × mapHeight+1 可保证整个画面都能被打印
//CONSOLE_CURSOR_INFO cursorInfo; 控制控制台光标的外观，bVisible = false 让光标消失。游戏刷新频率很高，闪烁的光标会非常分散注意力
//SetConsoleCursorPosition(hConsole, coord); 在 setCur(0, 0) 里，它将光标重置到左上角，使新的地图帧覆盖旧画面，而不是往下滚动，避免了画面撕裂和闪烁
//if (GetAsyncKeyState(VK_SPACE) & 0x8000) 直接查询硬件层某个按键的当前状态，不依赖消息队列。返回值的最高位（0x8000）为 1 表示该键正在被按住
//常用虚拟键码：VK_SPACE（空格）、VK_ESCAPE（ESC）、VK_LEFT/VK_RIGHT（方向键）
//Sleep - 控制帧率，让当前线程挂起指定毫秒数

/*
1.初始化：获取句柄 → 设缓冲区容纳整张地图 → 隐藏光标。

2.主循环每一帧：
	GetAsyncKeyState 处理输入
	更新物理/碰撞
	绘制到 map[][] 数组
	setCur(0,0) 光标归零 → ShowMap 打印地图（覆盖旧帧）
	Sleep(10) 控制节奏

3.结束：释放动态内存。
*/

#define mapWidth 256
#define mapHeight 25

// ========== 游戏物理参数 ==========
const float GRAVITY              = 0.08f;   // 重力加速度
const float JUMP_SPEED           = -1.2f;   // 玩家跳跃初速度
const float COIN_BOUNCE_SPEED    = -0.7f;   // 金币弹跳初速度
const float ENEMY_HORIZ_SPEED    = 0.2f;    // 敌人水平巡逻速度

// ========== 显示参数 ==========
const int VIEW_WIDTH  = 80;    // 控制台窗口宽度（列）
const int VIEW_HEIGHT = 25;    // 控制台窗口高度（行）

// ========== 时间参数（毫秒） ==========
const int FRAME_DELAY          = 10;    // 每帧延时
const int DEATH_DELAY          = 1000;  // 死亡画面持续时间
const int LEVEL_COMPLETE_DELAY = 500;   // 过关画面持续时间

// ========== 分数参数 ==========
const int SCORE_STOMP = 50;   // 踩死敌人得分
const int SCORE_COIN  = 100;  // 吃金币得分

typedef int BOOL;

#define TRUE 1
#define FALSE 0

typedef struct SObject //typedef 是关键字，意思是“给一个已有的类型起个新名字”
{
	float x,y; //浮点型，移动更流畅
	float width, height;
	float vertSpeed;
	BOOL IsFly;
	char cType;
	float horizSpeed; //物体的水平移动速度，敌人和道具也能自己移动
} TObject; //物体的位置，告诉编译器：“以后用 TObject 就等价于 struct SObject。”

char map[mapHeight][mapWidth+1]; //最后一列存'\0'
TObject Hanzio;

TObject *brick = NULL; //砖块动态数组指针
int brickLength;

TObject *moving = NULL; //动态数组指针，存放敌人和道具
int movingLength;


int level = 1; //当前关卡
int score;
int maxLvl;

void ClearMap()
{
	for (int i = 0; i < mapWidth; i++)
		map[0][i] = ' ';
	map[0][mapWidth] = '\0';
	for (int j = 1; j < mapHeight; j++)
		std::sprintf(map[j], map[0]); //把格式化的字符串写入到目标数组
	//snprintf?
}

/*
void ClearMap()
{
    for (int j = 0; j < mapHeight; j++)
    {
        // 用 memset 把一整行快速填充为空格
        memset(map[j], ' ', mapWidth);
        map[j][mapWidth] = '\0';   // 末尾加结束符
    }
}
*/

void ShowMap()
{
    const int viewWidth = VIEW_WIDTH;
    int playerCol = (int)round(Hanzio.x);           // 四舍五入，消除抖动
    int startCol = playerCol - viewWidth / 2;

    if (startCol < 0) startCol = 0;
    if (startCol + viewWidth > mapWidth) startCol = mapWidth - viewWidth;

    for (int j = 0; j < mapHeight; j++)
    {
        map[j][startCol + viewWidth] = '\0';
        printf("%s\n", map[j] + startCol);
        map[j][startCol + viewWidth] = ' ';
    }
}

/*void ShowMap() 
{
	map[mapHeight-1][mapWidth-1] = '\0';
	for (int j = 0; j < mapHeight; j++)
		printf("%s\n", map[j]);
}
*/
/*void ShowMap()
{
    // 确保每行末尾有 '\0'，最后一行右下角也设为 '\0'
    map[mapHeight - 1][mapWidth - 1] = '\0';

    // 预计算总长度：每行 mapWidth + 1（换行符），最后再多一个 '\0'
    int totalLen = mapHeight * (mapWidth + 1);
    char* buffer = new char[totalLen + 1];  // +1 给最末尾的 '\0'

    int pos = 0;
    for (int j = 0; j < mapHeight; j++)
    {
        // 复制该行内容（不包含原有的 '\0'，因为我们手动填充）
        for (int i = 0; i < mapWidth; i++)
            buffer[pos++] = map[j][i];
        buffer[pos++] = '\n';   // 手动添加换行
    }
    buffer[pos] = '\0';         // 最终字符串结束符

    printf("%s", buffer);       // 一次性打印整个画面
    delete[] buffer;
}*/

void SetObjectPos(TObject *obj, float xPos, float yPos) //把任意一个游戏物体的坐标设置到指定的位置
{
	obj->x = xPos;
	obj->y = yPos;
}

void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType) // 一次性把一个游戏物体的所有基本属性初始化好
{
	SetObjectPos(obj, xPos, yPos);
	obj->width = oWidth;
	obj->height = oHeight;
	obj->vertSpeed = 0;
	obj->cType = inType;
	obj->horizSpeed = ENEMY_HORIZ_SPEED;
	obj->IsFly = FALSE;
}

BOOL IsCollision(TObject o1, TObject o2); //提前声明函数
void CreateLevel(int lvl);
TObject *GetNewMoving();

void PlayerDead()
{
	system("color 4F");
	Sleep(LEVEL_COMPLETE_DELAY);
	CreateLevel(level);
}

void VertMoveObject(TObject *obj) //先移动，再修正
{
	obj->IsFly = TRUE; //1.假设自己在空中，直接修改了对象的状态，状态每帧重置
	obj->vertSpeed += GRAVITY; //2.重力加速度，速度每帧增加
	SetObjectPos(obj, obj->x, obj->y + obj->vertSpeed); //3.用新速度更新位置（Y坐标，往下掉或者往上跳）、
	
	for (int i = 0; i < brickLength; i++) //4.遍历所有砖块，检查碰撞
	{
		if (IsCollision(*obj, brick[i]))
		{
			if ((brick[i].cType == '?') &&
				(obj->vertSpeed < 0) && //向上顶
				(obj == &Hanzio)) //比较两个指针是否相等，即是否指向同一个东西
			{
				brick[i].cType = '-'; //问号砖变成空砖
				InitObject(GetNewMoving(), brick[i].x, brick[i].y-3, 3, 2, '$');
				moving[movingLength - 1].vertSpeed = COIN_BOUNCE_SPEED; //GetNewMoving() 刚才把 movingLength 加 1 了，新元素正好在最后一个位置，索引就是 movingLength - 1
			}
			//撞到了：回退Y，停止速度，落地
			obj->y -= obj->vertSpeed; //撤回下落
			obj->vertSpeed = 0;
			obj->IsFly = FALSE;
			//如果撞的是过关砖块'+'，切换关卡
			if (brick[i].cType == '+')
			{
				level++;
				if (level > maxLvl) level = 1;
				system("color 2F"); //屏幕变绿，过关特效
				Sleep(LEVEL_COMPLETE_DELAY);
				CreateLevel(level);	
			}	
			break;
		}
	}
}

void DeleteMoving(int i) //敌人，道具的销毁函数
{
	movingLength--;
	moving[i] = moving[movingLength]; //用最后一个元素覆盖第 i 个元素
	moving = (TObject*)realloc(moving, sizeof(*moving) * movingLength); //释放多余的内存
}

/*
if (movingLength == 0) {
    free(moving);
    moving = NULL;
}
*/

void HanzioCollision() //玩家与环境互动
{
	for (int i = 0; i < movingLength; i++)
	{
		if (IsCollision(Hanzio, moving[i]))
		{
			if (moving[i].cType == 'o')
			{
				if ((Hanzio.IsFly == TRUE)
					&& (Hanzio.vertSpeed > 0) //玩家在下落
					&& (Hanzio.y + Hanzio.height < moving[i].y + moving[i].height * 0.5)) // 玩家从正上方踩到了敌人的头顶
				{
					score += SCORE_STOMP;
					DeleteMoving(i);
					i--; //删除元素后索引上的对象可能会被漏掉
					continue;
				} else
					PlayerDead();
			}
			
			if (moving[i].cType == '$')
			{
				score += SCORE_COIN;
				DeleteMoving(i);
				i--;
				continue;
			}
		}
	}
}

void HorizonMoveObject(TObject *obj) //动态道具水平移动
{
	obj->x += obj->horizSpeed; //假设敌人移动了
	for (int i = 0; i < brickLength; i++)
	{
		if (IsCollision(*obj, brick[i]))
		{
			obj->x -= obj->horizSpeed; //如果敌人和墙相撞，撤销移动
			obj->horizSpeed = -obj->horizSpeed; //修改运动方向
			return;
		}
	}
	if (obj->cType == 'o') //敌人没有撞墙，检测下一步会不会让它掉落
	{
		TObject tmp = *obj; //栈上声明创建局部对象，函数结束自动销毁。复制一份敌人，得到它在当前位置的副本
		VertMoveObject(&tmp); //给副本施加向下的速度
		if (tmp.IsFly == TRUE) //如果进入，就表示悬崖，因为如果碰撞，状态会被修改
		{
			obj->x -= obj->horizSpeed;
			obj->horizSpeed = -obj->horizSpeed;
		}
	}	
}

BOOL IsPosInMap(int x, int y) //边界检查，坐标是否在地图的有效范围内
{
	return ((x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight));
}

void PutObjectOnMap(TObject obj) //将游戏物体嵌入地图数组里，只读数据
{
	int ix = (int)round(obj.x);
	int iy = (int)round(obj.y);
	int iWidth = (int)round(obj.width);
	int iHeight = (int)round(obj.height);
	
	for (int i = ix; i < (ix + iWidth); i++) //注意，这里x,y是物体的左上角，像素起点
		for (int j = iy; j < (iy + iHeight); j++)
			if (IsPosInMap(i,j))
				map[j][i] = obj.cType;
}

void setCur(int x, int y) //把控制台的光标移动到指定的 (x, y) 坐标位置
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void HorizonMoveMap(float dx) //处理玩家和地图，主要是处理地图
{
	Hanzio.x -= dx; //1.玩家反向移动
	for (int i = 0; i < brickLength; i++) //2.检查是否撞到了任何砖块
		if (IsCollision(Hanzio, brick[i]))
		{
			Hanzio.x += dx; //撞到了，玩家退回原位
			return;
		}
	Hanzio.x += dx; //3.没撞到，玩家坐标恢复
	//4.移动所有砖块和敌人（世界朝玩家按键方向移动）
	for (int i = 0; i < brickLength; i++) //没有撞到
		brick[i].x += dx; //所有砖块反向移动
	for (int i = 0; i < movingLength; i++)
		moving[i].x += dx;
}

BOOL IsCollision(TObject o1, TObject o2)
{
	return ((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) && ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height));
}

TObject *GetNewBrick() //砖块工厂
{
	brickLength++;
	brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength); //在 C++ 里，void* 不能自动赋值给其他指针类型，必须手动转换
	return brick + brickLength - 1;
}

TObject *GetNewMoving()
{
	movingLength++;
	moving = (TObject*)realloc(moving, sizeof(*moving) * movingLength);
	return moving + movingLength - 1;
}

void PutScoreOnMap()
{
	char c[30]; //临时字符数组
	sprintf(c, "Score:%d", score);
	int len = strlen(c); //计算字符串长度
	for (int i = 0; i < len; i++)
	{
		map[1][i+5] = c[i];
	}
}

void CreateLevel(int lvl) //重置玩家，分配砖块数组，并把砖块一一摆好
{
	//1.设置背景色
	system("color 9F"); //color 是 Windows 控制台的内置命令，用来设置前景色和背景色
	//9 代表蓝色背景，F 代表亮白色前景
	
	//2.清空砖块数组
	free(brick);
	brick = NULL;
	brickLength = 0;
	//brick = (TObject*)realloc(brick,0); 安全隐患？
	
	
	//3.清空敌人/道具数组
	free(moving);
	moving = NULL;
	movingLength = 0;
	//moving = (TObject*)realloc(moving, 0);
	
	//4.重置玩家
	InitObject(&Hanzio, 10, 15, 3, 3, '@'); 
	
	//5.分数归零
	score = 0;
	
	//6.搭建场景
	
	if (lvl == 1)
	{
    // ========== 地面 ==========
    InitObject(GetNewBrick(), 0, 20, 200, 5, '#');

    // ========== 第一金币阵垫脚平台 ==========
    InitObject(GetNewBrick(), 30, 15, 35, 3, '#');

    // ===== 第一金币阵 =====
    InitObject(GetNewBrick(), 25, 6, 6, 4, '?');
    InitObject(GetNewBrick(), 40, 5, 6, 4, '?');
    InitObject(GetNewBrick(), 55, 4, 6, 4, '?');
    InitObject(GetNewBrick(), 70, 3, 6, 4, '?');

    // 衔接平台
    InitObject(GetNewBrick(), 75, 12, 25, 2, '#');

    // ===== 中段障碍 =====
    InitObject(GetNewBrick(), 80, 20, 20, 5, '#');
    InitObject(GetNewBrick(), 120, 15, 10, 10, '#');

    // ===== 第二金币阵 =====
    InitObject(GetNewBrick(), 100, 12, 35, 2, '#');   // 垫脚平台
    InitObject(GetNewBrick(), 97, 8, 5, 4, '#');      // 助跑平台1
    InitObject(GetNewBrick(), 99, 0, 4, 3, '?');
    InitObject(GetNewBrick(), 114, 1, 4, 3, '?');

    // ★ 浮空台阶（衔接第二金币阵与大平台）
    InitObject(GetNewBrick(), 110, 9, 5, 2, '#');

    // ===== 长平台（大平台） =====
    InitObject(GetNewBrick(), 115, 14, 7, 5, '#');

    // ===== 过关助跑平台 =====
    InitObject(GetNewBrick(), 190, 17, 15, 2, '#');

    // ========== 过关砖块 ==========
    InitObject(GetNewBrick(), 210, 15, 10, 10, '+');
	}
	
	if (lvl == 2)
	{
    // ========== 地面（分段制造死亡坑） ==========
    InitObject(GetNewBrick(), 0, 20, 60, 5, '#');
    InitObject(GetNewBrick(), 80, 20, 30, 5, '#');
    InitObject(GetNewBrick(), 150, 20, 60, 5, '#');

    // ========== 第一金币阵 ==========
    InitObject(GetNewBrick(), 20, 14, 25, 2, '#');
    InitObject(GetNewBrick(), 25, 2, 5, 4, '?');
    InitObject(GetNewBrick(), 30, 2, 5, 4, '?');
    InitObject(GetNewBrick(), 35, 2, 5, 4, '?');
    InitObject(GetNewBrick(), 40, 2, 5, 4, '?');

    // ★ 新增：第一金币阵的“活台阶”敌人
    InitObject(GetNewMoving(), 22, 9, 3, 2, 'o');   // 悬在平台和问号砖之间
    InitObject(GetNewMoving(), 38, 9, 3, 2, 'o');   // 对应右端

    // ========== 中段障碍 ==========
    InitObject(GetNewBrick(), 60, 15, 10, 10, '#');
    InitObject(GetNewBrick(), 120, 15, 10, 10, '#');

    // ========== 第二金币阵 ==========
    InitObject(GetNewBrick(), 85, 12, 10, 2, '#');
    InitObject(GetNewBrick(), 97, 9, 10, 2, '#');
    InitObject(GetNewBrick(), 114, 8, 5, 2, '#');
    InitObject(GetNewBrick(), 105, 0, 5, 3, '?');
    InitObject(GetNewBrick(), 112, 0, 5, 3, '?');

    // ★ 新增：台阶路线上可踩死的敌人
    InitObject(GetNewMoving(), 93, 10, 3, 2, 'o');   // 第一级台阶上方
    InitObject(GetNewMoving(), 100, 8, 3, 2, 'o');   // 第二级台阶上方
    InitObject(GetNewMoving(), 112, 6, 3, 2, 'o');   // 靠近高位问号砖

    // ★ 新增：坑上方的浮空敌人（高风险高回报）
    InitObject(GetNewMoving(), 70, 13, 3, 2, 'o');   // 坑1上方
    InitObject(GetNewMoving(), 130, 13, 3, 2, 'o');  // 坑2上方

    // ========== 地面巡逻敌人（保留） ==========
    InitObject(GetNewMoving(), 25, 19, 3, 2, 'o');
    InitObject(GetNewMoving(), 50, 19, 3, 2, 'o');
    InitObject(GetNewMoving(), 90, 19, 3, 2, 'o');
    InitObject(GetNewMoving(), 160, 19, 3, 2, 'o');
    InitObject(GetNewMoving(), 180, 19, 3, 2, 'o');

    // ========== 过关助跑平台 ==========
    InitObject(GetNewBrick(), 190, 17, 15, 2, '#');

    // ========== 过关砖块 ==========
    InitObject(GetNewBrick(), 210, 15, 10, 10, '+');
	}
	
	if (lvl == 3)
	{
    // ========== 地面 ==========
    InitObject(GetNewBrick(), 0, 20, 25, 5, '#');
    InitObject(GetNewBrick(), 35, 20, 25, 5, '#');
    InitObject(GetNewBrick(), 28, 24, 10, 1, '#');
    InitObject(GetNewBrick(), 70, 20, 50, 5, '#');
    InitObject(GetNewBrick(), 170, 20, 45, 5, '#');

    // ========== 凹字形楼阁 ==========
    InitObject(GetNewBrick(), 40, 13, 5, 12, '#');
    InitObject(GetNewBrick(), 75, 13, 5, 12, '#');
    InitObject(GetNewBrick(), 45, 16, 30, 2, '#');
    InitObject(GetNewBrick(), 55, 10, 10, 2, '?');
    InitObject(GetNewMoving(), 50, 14, 3, 2, 'o');
    InitObject(GetNewMoving(), 65, 14, 3, 2, 'o');

    // 过渡浮桥
    InitObject(GetNewBrick(), 85, 12, 15, 1, '#');

        // ========== 回字形迷宫（中心开门，中央大问号砖） ==========

    // 外圈底边：完整
    InitObject(GetNewBrick(), 100, 14, 50, 2, '#');

    // 外圈顶边：在中心位置（x=120~130）开一个缺口，方便从上方跳入
    InitObject(GetNewBrick(), 100, 0, 18, 2, '#');    // 左段 x=100~120
    InitObject(GetNewBrick(), 130, 0, 18, 2, '#');    // 右段 x=130~150
    // 中心 x=120~130 留空，形成上方入口

    // 外圈左墙：在中心高度（y=8~12）完全打开，形成正面入口
    InitObject(GetNewBrick(), 100, 6, 2, 2, '#');     // 顶部 y=4~8
    // y=8~12 留空作为正门入口
    InitObject(GetNewBrick(), 100, 12, 2, 2, '#');    // 底部 y=12~16

    // 外圈右墙：对称开口
    InitObject(GetNewBrick(), 148, 6, 2, 2, '#');
    // y=8~12 留空
    InitObject(GetNewBrick(), 148, 12, 2, 2, '#');

    // 内圈底边：在中心位置（x=120~130）打开缺口，进入内圈
    InitObject(GetNewBrick(), 110, 13, 10, 1, '#');   // 左段 x=110~120
    InitObject(GetNewBrick(), 130, 13, 10, 1, '#');   // 右段 x=130~140
    // 中心 x=120~130 留空，形成内圈入口

    // 内圈顶边：完整
    InitObject(GetNewBrick(), 110, 7, 30, 1, '#');


    // ★ 中央大型问号砖（放在内圈中心，y=9 是内圈的中心高度）
    InitObject(GetNewBrick(), 122, 8, 6, 2, '?');     // 宽6高4的大问号砖，占据 x=122~128, y=8~11

    // 内圈里的巡逻敌人（保护中央大问号砖）
    InitObject(GetNewMoving(), 118, 11, 3, 2, 'o');
    InitObject(GetNewMoving(), 130, 11, 3, 2, 'o');

    // 外圈里的问号砖（左侧和右侧）
	InitObject(GetNewBrick(), 104, 6, 4, 2, '?');
	InitObject(GetNewBrick(), 142, 6, 4, 2, '?');

	// 外圈左侧敌人
	InitObject(GetNewMoving(), 104, 10, 3, 2, 'o');

	// 外圈右侧敌人（站在外圈底边上）
	InitObject(GetNewMoving(), 141, 12, 3, 2, 'o');
	
    // 外圈顶下的奖励问号砖
    InitObject(GetNewBrick(), 121, 1, 8, 2, '?');
	
    // ========== 大坑金币阵（去掉直接生成的 $，改为问号砖） ==========
    InitObject(GetNewBrick(), 130, 24, 40, 2, '#');
    InitObject(GetNewMoving(), 138, 22, 3, 2, 'o');
    InitObject(GetNewMoving(), 152, 22, 3, 2, 'o');
    InitObject(GetNewBrick(), 140, 17, 5, 3, '?');
    InitObject(GetNewBrick(), 155, 16, 5, 3, '?');

    // ========== 过关 ==========
    InitObject(GetNewBrick(), 200, 17, 15, 2, '#');
    InitObject(GetNewBrick(), 215, 15, 10, 10, '+');
	}
	
	//7.设置最大关卡数
	maxLvl = 3;
}

int main() 
{
	// ===== 一、初始化阶段 =====
	system("chcp 437 > nul");   //切换控制台代码页为英文，防止中文输入法
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //获取控制台句柄

    // 1. 设置缓冲区大小（画布）
    COORD bufferSize = { mapWidth + 1, mapHeight + 1 };
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // 2. 设置窗口可视区域（相框）
    SMALL_RECT windowRect = { 0, 0, VIEW_WIDTH - 1, VIEW_HEIGHT - 1 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowRect);

    // 3. 禁用鼠标和快速编辑
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode(hInput, &consoleMode);
    consoleMode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_MOUSE_INPUT);
    SetConsoleMode(hInput, consoleMode);

    // 4. 隐藏光标
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // 5. 加载第一关
    CreateLevel(level);
	
	// ===== 二、游戏循环 =====
	do
	{
		//1.清空画布
		ClearMap();
		
		//2.处理输入
		if ((Hanzio.IsFly == FALSE) && (GetAsyncKeyState(VK_SPACE) & 0x8000)) Hanzio.vertSpeed = JUMP_SPEED;
		if (GetAsyncKeyState('A') & 0x8000) HorizonMoveMap(1);   // 向左
		if (GetAsyncKeyState('D') & 0x8000) HorizonMoveMap(-1);    // 向右
		
		//3.垂直物理
		VertMoveObject(&Hanzio); 
		
		//4.边界检查
		if (Hanzio.y > mapHeight) PlayerDead();
		
		//5.物理更新
		HanzioCollision();
		
		//6. 绘制砖块
		for (int i = 0; i < brickLength; i++)
			PutObjectOnMap(brick[i]);
		
		//7. 更新并绘制敌人/道具
		for (int i = 0; i < movingLength; i++)
		{
			VertMoveObject(moving + i); //敌人重力下落
			HorizonMoveObject(moving + i); //敌人水平巡逻
			if (moving[i].y > mapHeight) //掉出地图就删除
			{
				DeleteMoving(i);
				i--;
				continue;
			}
			PutObjectOnMap(moving[i]);
		}
		
		//8. 绘制玩家和分数
		PutObjectOnMap(Hanzio);
		PutScoreOnMap();
		
		//9.显示到屏幕
		setCur(0,0);
		ShowMap();

		//10.控制帧率
		Sleep(FRAME_DELAY);
	}
	while (GetAsyncKeyState(VK_ESCAPE) >= 0);
	
	return 0;
}
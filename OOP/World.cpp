#include "World.hpp"
#include "Constants.hpp"

World::World() : currentLevel(1), score(0) {
    LoadLevel(1);
}

void World::LoadLevel(int lvl) {
    bricks.clear();
    questionBlocks.clear();
    coins.clear();
    enemies.clear();
    currentLevel = lvl;
    score = 0;

    player.SetX(5);
    player.SetY(19);
    player.SetVertSpeed(0);
    player.SetOnGround(false);

    if (lvl == 1) LoadLevel1();
    else if (lvl == 2) LoadLevel2();
    else if (lvl == 3) LoadLevel3();
}

void World::LoadLevel1() {
    // 地面（全实心）
    bricks.emplace_back(0, 22, 200, 5, '#');

    // 第一金币阵垫脚平台（可穿越，需要跳上去站）
    bricks.emplace_back(30, 15, 35, 3, '#',true);

    // 问号砖（问号砖本身就有独立碰撞，这里不涉及）
    questionBlocks.emplace_back(25, 9, 6, 4);
    questionBlocks.emplace_back(40, 8, 6, 4);
    questionBlocks.emplace_back(55, 7, 6, 4);
    questionBlocks.emplace_back(70, 6, 6, 4);

    // 衔接平台（可穿越）
    bricks.emplace_back(75, 12, 25, 2, '#');

    // 中段障碍（全实心，这是障碍物，应该撞头）
    bricks.emplace_back(80, 20, 20, 5, '#');
    bricks.emplace_back(120, 15, 10, 10, '#');

    // 第二金币阵垫脚平台（可穿越）
    bricks.emplace_back(100, 12, 35, 2, '#',true);

    // 第二金币阵问号砖
    questionBlocks.emplace_back(99, 3, 4, 3);
    questionBlocks.emplace_back(114, 4, 4, 3);

    // 浮空台阶
    bricks.emplace_back(110, 9, 5, 2, '#');

    // 长平台
    bricks.emplace_back(115, 14, 7, 5, '#');

    // 过关助跑平台
    bricks.emplace_back(190, 17, 15, 2, '#');

    // 过关砖块（全实心，碰到就要能站上去）
    bricks.emplace_back(210, 15, 10, 10, '+');
}

void World::LoadLevel2() {
    // ========== 地面（分段制造死亡坑） ==========
    bricks.emplace_back(0, 22, 60, 3, '#');
    bricks.emplace_back(80, 22, 30, 3, '#');
    bricks.emplace_back(150, 22, 60, 3, '#');

    // ========== 第一金币阵 ==========
    // 垫脚平台（可穿越）
	bricks.emplace_back(9, 16, 18, 2, '#', true);
    bricks.emplace_back(23, 14, 25, 2, '#', true);
    // 问号砖（悬空在平台上方）
    questionBlocks.emplace_back(25, 4, 5, 4); // 原来是 y=2, 5是合理的起跳高度
    questionBlocks.emplace_back(30, 4, 5, 4);
    questionBlocks.emplace_back(35, 4, 5, 4);
    questionBlocks.emplace_back(40, 4, 5, 4);

    // ★ 第一金币阵的“活台阶”敌人（悬在平台和问号砖之间）
    // 站在平台上 (y=14) 时，敌人高度 (y=12) 正好需要踩死才能过去
    enemies.emplace_back(22, 12); 
    enemies.emplace_back(38, 12);

    // ========== 中段障碍 ==========
    bricks.emplace_back(60, 15, 10, 10, '#');
    bricks.emplace_back(120, 15, 10, 10, '#');

    // ========== 第二金币阵（台阶式结构） ==========
    // 台阶（可穿越）
    bricks.emplace_back(75, 12, 10, 2, '#', true);   // 第一级台阶
    bricks.emplace_back(97, 9, 10, 2, '#', true);    // 第二级台阶
    bricks.emplace_back(114, 8, 5, 2, '#', true);    // 第三级台阶
    // 问号砖（高处奖励）
    questionBlocks.emplace_back(105, 4, 5, 3);       // 需要从第三级台阶起跳
    questionBlocks.emplace_back(112, 4, 5, 3);

    // ★ 台阶路线上可踩死的敌人（增加台阶攀登难度）
    enemies.emplace_back(93, 10);   // 第一级台阶上方
    enemies.emplace_back(100, 7);   // 第二级台阶上方
    enemies.emplace_back(112, 6);   // 靠近高位问号砖

    // ★ 坑上方的浮空敌人（高风险高回报）
    enemies.emplace_back(70, 13);   // 坑1上方
    enemies.emplace_back(130, 13);  // 坑2上方

    // ========== 地面巡逻敌人 ==========
    // 站在地面上 (y=22)，所以敌人 Y 坐标 = 地面顶部 - 敌人高度 = 22 - 2 = 20
    enemies.emplace_back(25, 20);
    enemies.emplace_back(50, 20);
    enemies.emplace_back(90, 20);
    enemies.emplace_back(160, 20);
    enemies.emplace_back(180, 20);

    // ========== 过关区域 ==========
    bricks.emplace_back(190, 17, 15, 2, '#', true); // 过关助跑平台
    bricks.emplace_back(210, 15, 10, 10, '+');       // 过关砖块
}

void World::LoadLevel3() {
    // ========== 分段地面 ==========
    bricks.emplace_back(0, 22, 25, 5, '#');
    bricks.emplace_back(35, 22, 25, 5, '#');
    bricks.emplace_back(28, 24, 10, 1, '#', true);  // 连接浮桥
    bricks.emplace_back(70, 22, 50, 5, '#');
    bricks.emplace_back(170, 22, 45, 5, '#');

    // ========== 凹字形楼阁 ==========
    // 左右高墙（实体）
    bricks.emplace_back(40, 11, 5, 11, '#');
    bricks.emplace_back(75, 11, 5, 11, '#');
    // 内部平台（可穿越）
    bricks.emplace_back(45, 16, 30, 2, '#', true);
    // 楼阁内的问号砖
    questionBlocks.emplace_back(55, 10, 10, 2);
    // 楼阁内的巡逻敌人（站在内部平台上，平台顶部y=16，敌人高2，敌人Y=14）
    enemies.emplace_back(50, 14);
    enemies.emplace_back(65, 14);

    // ========== 过渡浮桥 ==========
    bricks.emplace_back(85, 12, 15, 1, '#', true);

    // ========== 回字形迷宫 ==========
    // 外圈底边：完整
    bricks.emplace_back(100, 14, 50, 2, '#');
    // 外圈顶边：在中心位置（x=120~130）开缺口
    bricks.emplace_back(100, 4, 18, 2, '#');
    bricks.emplace_back(130, 4, 18, 2, '#');

    // 外圈左墙：?-交替花纹，中间开门（y=8~12）
    bricks.emplace_back(100, 4, 2, 2, '-');
    bricks.emplace_back(100, 6, 2, 2, '?');
    bricks.emplace_back(100, 8, 2, 2, '-');
    bricks.emplace_back(100, 10, 2, 2, '?');
    bricks.emplace_back(100, 12, 2, 2, '-');

    // 外圈右墙：对称花纹
    bricks.emplace_back(148, 4, 2, 2, '-');
    bricks.emplace_back(148, 6, 2, 2, '?');
    bricks.emplace_back(148, 8, 2, 2, '-');
    bricks.emplace_back(148, 10, 2, 2, '?');
    bricks.emplace_back(148, 12, 2, 2, '-');

    // 内圈底边：中心开口（x=120~130）
    bricks.emplace_back(110, 12, 10, 2, '#');
    bricks.emplace_back(130, 12, 10, 2, '#');
    // 内圈顶边：完整
    bricks.emplace_back(110, 7, 30, 2, '#');
    // 内圈左右墙（小段）
    bricks.emplace_back(110, 7, 2, 3, '#');
    bricks.emplace_back(138, 7, 2, 3, '#');

    // ★ 中央大型问号砖（内圈中心）
    questionBlocks.emplace_back(122, 8, 6, 2);

    // 内圈巡逻敌人（保护大问号砖，站在内底边 y=12 上方，敌人 Y=10）
    enemies.emplace_back(118, 10);
    enemies.emplace_back(130, 10);

    // 外圈问号砖（左右两侧）
    questionBlocks.emplace_back(104, 6, 4, 2);
    questionBlocks.emplace_back(142, 6, 4, 2);

    // 外圈左侧敌人（站在外圈底边 y=14 上方，敌人 Y=12）
    enemies.emplace_back(104, 12);
    // 外圈右侧敌人
    enemies.emplace_back(141, 12);

    // 外圈顶下奖励问号砖
    questionBlocks.emplace_back(121, 1, 8, 2);

    // ========== 大坑金币阵 ==========
    bricks.emplace_back(130, 24, 40, 2, '#', true);
    // 大坑上方的问号砖
    questionBlocks.emplace_back(140, 17, 5, 3);
    questionBlocks.emplace_back(155, 16, 5, 3);
    // 大坑附近的巡逻敌人（站在地面上，地面顶部 y=22，敌人 Y=20）
    enemies.emplace_back(138, 20);
    enemies.emplace_back(152, 20);

    // ========== 过关区域 ==========
    bricks.emplace_back(200, 17, 15, 2, '#', true);
    bricks.emplace_back(215, 15, 10, 10, '+');
}

void World::Update() {
    player.Update();

    // ========== 玩家与普通砖块垂直碰撞 ==========
	player.SetOnGround(false);
	for (auto& brick : bricks) {
		float px = player.GetX(), py = player.GetY();
		float pw = player.GetWidth(), ph = player.GetHeight();
		float bx = brick.GetX(), by = brick.GetY();
		float bw = brick.GetWidth(), bh = brick.GetHeight();

		if (px + pw > bx && px < bx + bw && py + ph > by && py < by + bh) {
			if (brick.IsSolidTopOnly()) {
				// 可穿越平台：只有下落且脚底刚穿过平台顶部时才站住
				float prevFeetY = py + ph - player.GetVertSpeed(); // 上一帧脚底位置
				if (player.GetVertSpeed() > 0 && prevFeetY <= by + 0.1f) {
					player.SetY(by - ph);
					player.SetVertSpeed(0);
					player.SetOnGround(true);
				}
				// 否则穿过
			} else {
				// 实心砖块：全向阻挡（用距离判断）
				float feetToTop = py + ph - by;
				float headToBottom = (by + bh) - py;
				if (feetToTop < headToBottom) {
					player.SetY(by - ph);
					player.SetVertSpeed(0);
					player.SetOnGround(true);
				} else {
					player.SetY(by + bh);
					player.SetVertSpeed(0);
				}
			}

			// 过关
			if (brick.GetSymbol() == '+') {
				int next = currentLevel + 1;
				if (next > maxLevel) next = 1;
				LoadLevel(next);
				return;
			}
			break;
		}
	}

    // ========== 玩家与问号砖垂直碰撞 ==========
    for (auto& qb : questionBlocks) {
        float px = player.GetX(), py = player.GetY();
        float pw = player.GetWidth(), ph = player.GetHeight();
        float bx = qb.GetX(), by = qb.GetY();
        float bw = qb.GetWidth(), bh = qb.GetHeight();

        if (px + pw > bx && px < bx + bw && py + ph > by && py < by + bh) {
            // 顶出金币（只有活的问号砖，且玩家从下方顶）
            if (qb.IsAlive() && player.GetVertSpeed() < 0) {
                qb.SetAlive(false);
                coins.emplace_back(bx + 1, by - 2);
            }

            // 碰撞处理（问号砖全按实体处理）
            float feetToTop = py + ph - by;
            float headToBottom = (by + bh) - py;
            if (feetToTop < headToBottom) {
                player.SetY(by - ph);
                player.SetVertSpeed(0);
                player.SetOnGround(true);
            } else {
                player.SetY(by + bh);
                player.SetVertSpeed(0);
            }
            break;
        }
    }

    // ========== 金币更新与收集 ==========
    for (auto& coin : coins) {
        coin.Update();
        float px = player.GetX(), py = player.GetY();
        float pw = player.GetWidth(), ph = player.GetHeight();
        float cx = coin.GetX(), cy = coin.GetY();
        float cw = coin.GetWidth(), ch = coin.GetHeight();
        if (coin.IsActive() && px + pw > cx && px < cx + cw && py + ph > cy && py < cy + ch) {
            coin.SetActive(false);
            score += 100;
        }
    }
    coins.erase(std::remove_if(coins.begin(), coins.end(),
        [](Coin& c) { return !c.IsActive(); }), coins.end());

    // ========== 敌人更新与碰撞 ==========
    for (auto& enemy : enemies) {
        if (!enemy.IsActive()) continue;
        enemy.Update();

        // 敌人垂直碰撞
        enemy.SetOnGround(false);
        for (auto& brick : bricks) {
            float ex = enemy.GetX(), ey = enemy.GetY();
            float ew = enemy.GetWidth(), eh = enemy.GetHeight();
            float bx = brick.GetX(), by = brick.GetY();
            float bw = brick.GetWidth(), bh = brick.GetHeight();

            if (ex + ew > bx && ex < bx + bw && ey + eh > by && ey < by + bh) {
                if (enemy.GetVertSpeed() >= 0) {
                    enemy.SetY(by - eh);
                    enemy.SetVertSpeed(0);
                    enemy.SetOnGround(true);
                } else {
                    enemy.SetY(by + bh);
                    enemy.SetVertSpeed(0);
                }
                break;
            }
        }

        HandleEnemyCollision(enemy);

        if (enemy.GetY() > MAP_HEIGHT) {
            enemy.SetActive(false);
        }
    }
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](Enemy& e) { return !e.IsActive(); }), enemies.end());

    // 玩家与敌人碰撞
    HandlePlayerEnemyCollisions();

    // 玩家掉落重置
    if (player.GetY() > MAP_HEIGHT) {
        LoadLevel(currentLevel);
    }
}

void World::MovePlayer(float dx) {
    // 保存原始位置
    float originalX = player.GetX();

    // 暂时抬高玩家，避免脚下地面干扰水平碰撞检测
    player.SetY(player.GetY() - 1.0f);
    player.MoveHorizontal(dx);

    bool blocked = false;
    for (auto& brick : bricks) {
        if (player.GetX() + player.GetWidth() > brick.GetX() &&
            player.GetX() < brick.GetX() + brick.GetWidth() &&
            player.GetY() + player.GetHeight() > brick.GetY() &&
            player.GetY() < brick.GetY() + brick.GetHeight()) {
            blocked = true;
            break;
        }
    }

    // 恢复垂直位置
    player.SetY(player.GetY() + 1.0f);

    if (blocked) {
        // 撞墙，回退到原始位置
        player.SetX(originalX);
    }
}

void World::Draw(Renderer& renderer) {
    for (auto& brick : bricks) brick.Draw(renderer);
    for (auto& qb : questionBlocks) qb.Draw(renderer);
    for (auto& coin : coins) coin.Draw(renderer);
    for (auto& enemy : enemies) enemy.Draw(renderer);
    player.Draw(renderer);
    renderer.DrawScore(score);
}

void World::HandleEnemyCollision(Enemy& enemy) {
    for (auto& brick : bricks) {
        float ex = enemy.GetX(), ey = enemy.GetY();
        float ew = enemy.GetWidth(), eh = enemy.GetHeight();
        float bx = brick.GetX(), by = brick.GetY();
        float bw = brick.GetWidth(), bh = brick.GetHeight();

        if (ex + ew > bx && ex < bx + bw && ey + eh > by && ey < by + bh) {
            enemy.SetX(ex - enemy.GetHorizSpeed());
            enemy.ReverseDirection();
            return;
        }
    }

    Enemy copy = enemy;
    copy.SetX(copy.GetX() + copy.GetHorizSpeed());
    copy.Update();
    bool willFall = true;
    for (auto& brick : bricks) {
        float cx = copy.GetX(), cy = copy.GetY();
        float cw = copy.GetWidth(), ch = copy.GetHeight();
        float bx = brick.GetX(), by = brick.GetY();
        float bw = brick.GetWidth(), bh = brick.GetHeight();
        if (cx + cw > bx && cx < bx + bw && cy + ch > by && cy < by + bh) {
            willFall = false;
            break;
        }
    }
    if (willFall) {
        enemy.SetX(enemy.GetX() - enemy.GetHorizSpeed());
        enemy.ReverseDirection();
    }
}

void World::HandlePlayerEnemyCollisions() {
    for (auto& enemy : enemies) {
        if (!enemy.IsActive()) continue;
        float px = player.GetX(), py = player.GetY();
        float pw = player.GetWidth(), ph = player.GetHeight();
        float ex = enemy.GetX(), ey = enemy.GetY();
        float ew = enemy.GetWidth(), eh = enemy.GetHeight();

        if (px + pw > ex && px < ex + ew && py + ph > ey && py < ey + eh) {
            if (player.GetVertSpeed() > 0 && py + ph < ey + eh * 0.5f) {
                enemy.SetActive(false);
                score += 50;
            } else {
                LoadLevel(currentLevel);
                return;
            }
        }
    }
}
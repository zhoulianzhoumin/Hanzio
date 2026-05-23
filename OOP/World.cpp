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
    bricks.emplace_back(0, 22, 200, 5, '#');
    bricks.emplace_back(30, 15, 35, 3, '#', true);
    questionBlocks.emplace_back(25, 9, 6, 4);
    questionBlocks.emplace_back(40, 8, 6, 4);
    questionBlocks.emplace_back(55, 7, 6, 4);
    questionBlocks.emplace_back(70, 6, 6, 4);
    bricks.emplace_back(75, 12, 25, 2, '#', true);
    bricks.emplace_back(80, 20, 20, 5, '#');
    bricks.emplace_back(120, 15, 10, 10, '#');
    bricks.emplace_back(100, 12, 35, 2, '#', true);
    questionBlocks.emplace_back(99, 3, 4, 3);
    questionBlocks.emplace_back(114, 4, 4, 3);
    bricks.emplace_back(110, 9, 5, 2, '#', true);
    bricks.emplace_back(115, 14, 7, 5, '#', true);
    bricks.emplace_back(190, 17, 15, 2, '#', true);
    bricks.emplace_back(210, 15, 10, 10, '+');
}

void World::LoadLevel2() {
    // 分段地面（死亡坑）
    bricks.emplace_back(0, 22, 60, 3, '#');
    bricks.emplace_back(80, 22, 30, 3, '#');
    bricks.emplace_back(150, 22, 60, 3, '#');

    // 第一金币阵平台（可穿越）
    bricks.emplace_back(20, 14, 25, 2, '#', true);

    // 第一金币阵问号砖
    questionBlocks.emplace_back(25, 5, 5, 4);
    questionBlocks.emplace_back(30, 5, 5, 4);
    questionBlocks.emplace_back(35, 5, 5, 4);
    questionBlocks.emplace_back(40, 5, 5, 4);

    // 中段障碍（实体）
    bricks.emplace_back(60, 15, 10, 10, '#');
    bricks.emplace_back(120, 15, 10, 10, '#');

    // 第二金币阵台阶
    bricks.emplace_back(85, 12, 10, 2, '#', true);
    bricks.emplace_back(97, 9, 10, 2, '#', true);
    bricks.emplace_back(114, 8, 5, 2, '#', true);

    // 第二金币阵问号砖
    questionBlocks.emplace_back(105, 3, 5, 3);
    questionBlocks.emplace_back(112, 3, 5, 3);

    // 坑上方浮空敌人
    enemies.emplace_back(70, 13);
    enemies.emplace_back(130, 13);

    // 地面巡逻敌人
    enemies.emplace_back(25, 20);
    enemies.emplace_back(50, 20);
    enemies.emplace_back(90, 20);
    enemies.emplace_back(160, 20);
    enemies.emplace_back(180, 20);

    // 过关助跑平台（可穿越）
    bricks.emplace_back(190, 17, 15, 2, '#', true);

    // 过关砖块
    bricks.emplace_back(210, 15, 10, 10, '+');
}

void World::LoadLevel3() {
    bricks.emplace_back(0, 22, 25, 5, '#');
    bricks.emplace_back(35, 22, 25, 5, '#');
    bricks.emplace_back(70, 22, 50, 5, '#');
    bricks.emplace_back(170, 22, 45, 5, '#');
    bricks.emplace_back(40, 11, 5, 11, '#');
    bricks.emplace_back(75, 11, 5, 11, '#');
    bricks.emplace_back(45, 16, 30, 2, '#', true);
    questionBlocks.emplace_back(55, 10, 10, 2);
    bricks.emplace_back(85, 12, 15, 1, '#', true);
    bricks.emplace_back(100, 14, 50, 2, '#');
    bricks.emplace_back(100, 4, 50, 2, '#');
    bricks.emplace_back(100, 4, 2, 10, '#');
    bricks.emplace_back(148, 4, 2, 10, '#');
    bricks.emplace_back(110, 12, 30, 2, '#', true);
    bricks.emplace_back(110, 7, 30, 2, '#', true);
    questionBlocks.emplace_back(122, 8, 6, 2);
    questionBlocks.emplace_back(104, 6, 4, 2);
    questionBlocks.emplace_back(142, 6, 4, 2);
    bricks.emplace_back(130, 24, 40, 2, '#', true);
    questionBlocks.emplace_back(140, 17, 5, 3);
    questionBlocks.emplace_back(155, 16, 5, 3);
    bricks.emplace_back(200, 17, 15, 2, '#', true);
    bricks.emplace_back(215, 15, 10, 10, '+');
}

void World::Update() {
    player.Update();

    player.SetOnGround(false);
    for (auto& brick : bricks) {
        float px = player.GetX(), py = player.GetY();
        float pw = player.GetWidth(), ph = player.GetHeight();
        float bx = brick.GetX(), by = brick.GetY();
        float bw = brick.GetWidth(), bh = brick.GetHeight();

        if (px + pw > bx && px < bx + bw && py + ph > by && py < by + bh) {
            if (brick.IsSolidTopOnly()) {
                if (player.GetVertSpeed() > 0 && py + ph - player.GetVertSpeed() <= by + 0.1f) {
                    player.SetY(by - ph);
                    player.SetVertSpeed(0);
                    player.SetOnGround(true);
                }
            } else {
                if (player.GetVertSpeed() >= 0) {
                    player.SetY(by - ph);
                    player.SetVertSpeed(0);
                    player.SetOnGround(true);
                } else {
                    player.SetY(by + bh);
                    player.SetVertSpeed(0);
                }
            }
            if (brick.GetSymbol() == '+') {
                int nextLevel = currentLevel + 1;
                if (nextLevel > maxLevel) nextLevel = 1;
                LoadLevel(nextLevel);
                return;
            }
            break;
        }
    }

    for (auto& qb : questionBlocks) {
        float px = player.GetX(), py = player.GetY();
        float pw = player.GetWidth(), ph = player.GetHeight();
        float bx = qb.GetX(), by = qb.GetY();
        float bw = qb.GetWidth(), bh = qb.GetHeight();

        if (px + pw > bx && px < bx + bw && py + ph > by && py < by + bh) {
            if (qb.IsAlive() && player.GetVertSpeed() < 0) {
                qb.SetAlive(false);
                coins.emplace_back(bx + 1, by - 2);
            }
            if (player.GetVertSpeed() >= 0) {
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

    for (auto& enemy : enemies) {
        if (!enemy.IsActive()) continue;
        enemy.Update();

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

    HandlePlayerEnemyCollisions();

    if (player.GetY() > MAP_HEIGHT) {
        LoadLevel(currentLevel);
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
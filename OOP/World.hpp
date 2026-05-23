#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <algorithm>
#include "Player.hpp"
#include "Brick.hpp"
#include "QuestionBlock.hpp"
#include "Coin.hpp"
#include "Enemy.hpp"

class World {
private:
    Player player;
    std::vector<Brick> bricks;
    std::vector<QuestionBlock> questionBlocks;
    std::vector<Coin> coins;
    std::vector<Enemy> enemies;
    int currentLevel;
    int score;
    int maxLevel = 3;

    void LoadLevel1();
    void LoadLevel2();
    void LoadLevel3();

    void HandleEnemyCollision(Enemy& enemy);
    void HandlePlayerEnemyCollisions();

public:
    World();
    void LoadLevel(int lvl);
    void Update();
    void Draw(Renderer& renderer);

    Player& GetPlayer() { return player; }
    int GetCurrentLevel() const { return currentLevel; }
    int GetScore() const { return score; }
};

#endif
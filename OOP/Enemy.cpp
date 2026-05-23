#include "Enemy.hpp"

Enemy::Enemy(float startX, float startY)
    : x(startX), y(startY), width(3), height(2),
      vertSpeed(0), horizSpeed(0.2f), active(true), onGround(false) {}

void Enemy::Update() {
    vertSpeed += GRAVITY;
    y += vertSpeed;
    x += horizSpeed;
}

void Enemy::Draw(Renderer& renderer) const {
    if (active) renderer.DrawRect(x, y, width, height, 'o');
}
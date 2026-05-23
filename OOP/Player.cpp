#include "Player.hpp"
#include "Constants.hpp"

Player::Player() : x(5), y(19), width(3), height(3), vertSpeed(0), onGround(false) {}

void Player::MoveHorizontal(float dx) {
    x += dx;
    if (x < 0) x = 0;
    if (x + width > MAP_WIDTH) x = MAP_WIDTH - width;
}

void Player::Jump() {
    if (onGround) {
        vertSpeed = JUMP_V;
        onGround = false;
    }
}

void Player::Update() {
    vertSpeed += GRAVITY;
    y += vertSpeed;
}

void Player::Draw(Renderer& renderer) const {
    renderer.DrawRect(x, y, width, height, '@');
}
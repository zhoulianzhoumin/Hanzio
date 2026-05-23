#include "Coin.hpp"
#include "Constants.hpp"

Coin::Coin(float startX, float startY)
    : x(startX), y(startY), width(3), height(2), vertSpeed(-0.7f), active(true) {}

void Coin::Update() {
    if (!active) return;
    vertSpeed += GRAVITY;
    y += vertSpeed;
}

void Coin::Draw(Renderer& renderer) const {
    if (active) renderer.DrawRect(x, y, width, height, '$');
}
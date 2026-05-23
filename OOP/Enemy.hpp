#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Renderer.hpp"
#include "Constants.hpp"

class Enemy {
private:
    float x, y;
    float width, height;
    float vertSpeed;
    float horizSpeed;
    bool active;
    bool onGround;

public:
    Enemy(float startX, float startY);

    void Update();
    void Draw(Renderer& renderer) const;

    bool IsActive() const { return active; }
    void SetActive(bool a) { active = a; }

    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }
    float GetVertSpeed() const { return vertSpeed; }
    float GetHorizSpeed() const { return horizSpeed; }
    bool IsOnGround() const { return onGround; }

    void SetX(float nx) { x = nx; }
    void SetY(float ny) { y = ny; }
    void SetVertSpeed(float v) { vertSpeed = v; }
    void SetOnGround(bool g) { onGround = g; }
    void ReverseDirection() { horizSpeed = -horizSpeed; }
};

#endif
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Renderer.hpp"

class Player {
private:
    float x, y;
    float width, height;
    float vertSpeed;
    bool onGround;

public:
    Player();

    void MoveHorizontal(float dx);
    void Jump();
    void Update();
    void Draw(Renderer& renderer) const;

    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }
    float GetVertSpeed() const { return vertSpeed; }

    void SetX(float nx) { x = nx; }
    void SetY(float ny) { y = ny; }
    void SetVertSpeed(float v) { vertSpeed = v; }
    void SetOnGround(bool g) { onGround = g; }
    bool IsOnGround() const { return onGround; }
};

#endif
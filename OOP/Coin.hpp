#ifndef COIN_HPP
#define COIN_HPP

#include "Renderer.hpp"

class Coin {
private:
    float x, y;
    float width, height;
    float vertSpeed;
    bool active;

public:
    Coin(float startX, float startY);

    bool IsActive() const { return active; }
    void SetActive(bool a) { active = a; }

    void Update();
    void Draw(Renderer& renderer) const;

    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }
};

#endif
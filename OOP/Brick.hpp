#ifndef BRICK_HPP
#define BRICK_HPP

#include "Renderer.hpp"

class Brick {
private:
    float x, y, width, height;
    char symbol;
    bool solidTopOnly;

public:
    Brick(float x, float y, float w, float h, char sym, bool topOnly = false)
        : x(x), y(y), width(w), height(h), symbol(sym), solidTopOnly(topOnly) {}

    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }
    char GetSymbol() const { return symbol; }
    bool IsSolidTopOnly() const { return solidTopOnly; }

    virtual void Draw(Renderer& renderer) const {
        renderer.DrawRect(x, y, width, height, symbol);
    }
};

#endif
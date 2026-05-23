#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "Renderer.hpp"

class GameObject {
protected:
    float x, y;
    float width, height;

public:
    GameObject(float x, float y, float w, float h);

    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }

    void SetX(float nx) { x = nx; }
    void SetY(float ny) { y = ny; }

    bool CollidesWith(const GameObject& other) const;
};

#endif
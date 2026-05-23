#include "GameObject.hpp"

GameObject::GameObject(float x, float y, float w, float h)
    : x(x), y(y), width(w), height(h) {}

bool GameObject::CollidesWith(const GameObject& other) const {
    return (x + width > other.x) &&
           (x < other.x + other.width) &&
           (y + height > other.y) &&
           (y < other.y + other.height);
}
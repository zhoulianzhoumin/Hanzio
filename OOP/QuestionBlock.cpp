#include "QuestionBlock.hpp"

QuestionBlock::QuestionBlock(float x, float y, float w, float h)
    : Brick(x, y, w, h, '?'), isAlive(true) {}

void QuestionBlock::Draw(Renderer& renderer) const {
    char sym = isAlive ? '?' : '-';
    renderer.DrawRect(GetX(), GetY(), GetWidth(), GetHeight(), sym);
}
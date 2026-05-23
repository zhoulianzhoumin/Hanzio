#ifndef QUESTIONBLOCK_HPP
#define QUESTIONBLOCK_HPP

#include "Brick.hpp"

class QuestionBlock : public Brick {
private:
    bool isAlive;

public:
    QuestionBlock(float x, float y, float w, float h);

    bool IsAlive() const { return isAlive; }
    void SetAlive(bool alive) { isAlive = alive; }

    void Draw(Renderer& renderer) const override;
};

#endif
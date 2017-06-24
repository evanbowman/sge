#pragma once

class Wall;
class Hero;

class Renderer {
public:
    virtual ~Renderer() {}
    virtual void Visit(const Wall& wall) = 0;
    virtual void Run() = 0;
};

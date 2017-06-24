#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "Renderer.hpp"

class RendererImpl : public Renderer {
public:
    RendererImpl(sf::RenderWindow& target);

    void Visit(const Wall& wall) override;
    void Run() override;
    
protected:
    sf::RenderWindow& m_target;
};

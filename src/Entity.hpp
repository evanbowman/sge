#pragma once

#include "GraphicsComponent.hpp"
#include "Types.hpp"

class Entity {
public:
    using GraphicsCompRef = std::unique_ptr<GraphicsComponent>;

    const Vec2& GetPosition() const;
    void SetPosition(const Vec2& position);

    void SetGraphicsComponent(GraphicsCompRef gfxComp);
    GraphicsComponent* GetGraphicsComponent();
    
private:
    Vec2 m_position;
    std::unique_ptr<GraphicsComponent> m_graphicsComponent;
};

#pragma once

#include "GraphicsComponent.hpp"
#include "Types.hpp"

class Entity {
public:
    using GraphicsCompRef = std::unique_ptr<GraphicsComponent>;

    Entity();
    
    const Vec2& GetPosition() const;
    void SetPosition(const Vec2& position);

    void SetGraphicsComponent(GraphicsCompRef gfxComp);
    GraphicsComponent* GetGraphicsComponent();

    void SetEnabled(bool enabled);
    bool IsEnabled() const;
    
private:
    Vec2 m_position;
    bool m_enabled;
    GraphicsCompRef m_graphicsComponent;
};

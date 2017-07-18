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

    bool HasAttribute(SGE_Attribute attrib) const;
    void AddAttribute(SGE_Attribute attrib);
    void RemoveAttribute(SGE_Attribute attrib);
    AttributeSet GetAttributeSet() const;
    void SetAttributeSet(const AttributeSet set);
    
private:
    Vec2 m_position;
    AttributeSet m_attributes;
    GraphicsCompRef m_graphicsComponent;
};

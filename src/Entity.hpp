#pragma once

#include "GraphicsComponent.hpp"
#include "Types.hpp"

class Entity {
public:
    using GraphicsCompRef = std::unique_ptr<GraphicsComponent>;

    explicit Entity(SGE_GUID handle);

    SGE_GUID GetId() const;
    
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
    const SGE_GUID m_handle;
    AttributeSet m_attributes;
    GraphicsCompRef m_graphicsComponent;
};

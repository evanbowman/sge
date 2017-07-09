#pragma once

/* GraphicsComponent.hpp
   This file contains the definition for all of the graphics
   components available within the engine. Each derived component
   must implement the TypeId method, this is required not by the
   engine's rendering code, but for the script API, for when it
   attempts to mutate a component's internal fields from the 
   higher opaque type GraphicsComponent. */

#include "Types.hpp"

class Sprite;
class Renderer;
class Animation;
class Entity;

using ZOrderIndex = int;

class GraphicsComponent {
public:
    GraphicsComponent();
    
    enum class Id {
        SpriteComponent,
        AnimationComponent
    };
    virtual void Display(Entity& entity,
                         Renderer& renderer) = 0;
    virtual Id TypeId() = 0;
    virtual ~GraphicsComponent() {}

    ZOrderIndex GetZOrder() const;
    void SetZOrder(ZOrderIndex zOrder);

    const Vec2& GetScale() const;
    void SetScale(const Vec2& scale);
    
protected:
    ZOrderIndex m_zOrder;
    Vec2 m_scale;
};

class SpriteComponent : public GraphicsComponent {
public:
    explicit SpriteComponent(Sprite* sprite);

    void Display(Entity& entity,
                 Renderer& renderer) override;
    Id TypeId() final;

    Sprite* GetSprite();
    
private:
    Sprite* m_sprite;
};

class AnimationComponent : public GraphicsComponent {
public:
    explicit AnimationComponent(Animation* animation);
    
    void Display(Entity& entity,
                 Renderer& renderer) override;
    Id TypeId() final;

    void SetKeyframe(size_t keyframe);
    size_t GetKeyframe() const;
    Animation* GetAnimation();
    
private:
    size_t m_keyframe;
    Animation* m_animation;
};

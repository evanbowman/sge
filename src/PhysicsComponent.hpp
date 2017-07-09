#pragma once

class Entity;

class PhysicsComponent {
public:
    enum class Id {
        Box2DComponent
    };

    virtual Id TypeId() const = 0;
    virtual ~PhysicsComponent() {}
};

class Box2DComponent : public PhysicsComponent {
public:
    Id TypeId() const override;
};

#pragma once

#include "Types.hpp"
#include "Entity.hpp"

class Wall : public Entity {
public:
    explicit Wall(const Vec2& position);

    void SendTo(Renderer& renderer) const override;
    void SendTo(Logic& logic) override;

    const Vec2& GetPosition();
};

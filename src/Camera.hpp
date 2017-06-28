#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

#include "Entity.hpp"

class Camera {
public:
    void SetTarget(std::shared_ptr<Entity> target);

private:
    std::weak_ptr<Entity> m_target;
};

#include "PhysicsComponent.hpp"

PhysicsComponent::Id Box2DComponent::TypeId() const {
    return Id::Box2DComponent;
}

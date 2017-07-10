#include "Camera.hpp"

void Camera::SetTarget(std::shared_ptr<Entity> target) {
    m_target = target;
}

void Camera::SetSpringiness(float springiness) {
    m_springiness = springiness;
}

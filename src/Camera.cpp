#include "Camera.hpp"

Camera::Camera(sf::RenderWindow& window) :
    m_window(window),
    m_view(m_window.getView()),
    m_springiness(2) {}

void Camera::SetTarget(std::shared_ptr<Entity> target) {
    m_target = target;
}

void Camera::SetSpringiness(float springiness) {
    m_springiness = springiness;
}

template <typename T> T lerp(const T & A, const T & B, const float t) {
    return A * t + (1 - t) * B;
}

void Camera::Update(USec dt) {
    if (auto targetSp = m_target.lock()) {
        const auto& pos = targetSp->GetPosition();
        const auto& center = m_view.getCenter();
        m_view.setCenter(lerp(pos, center,
                              dt * 0.000025 * (1.f / m_springiness)));
        m_window.setView(m_view);
    }
}

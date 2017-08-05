#include "Camera.hpp"

Camera::Camera() :
    m_springiness(2) {}

void Camera::Init(const sf::RenderWindow& window) {
    m_view = window.getView();
}

void Camera::SetTarget(std::shared_ptr<Entity> target) {
    m_target = target;
}

const sf::View& Camera::GetView() const {
    return m_view;
}

void Camera::SetSpringiness(float springiness) {
    m_springiness = springiness;
}

template <typename T> T lerp(const T & A, const T & B, const float t) {
    return A * t + (1 - t) * B;
}

void Camera::Update(USec dt, sf::RenderWindow& window) {
    if (auto targetSp = m_target.lock()) {
        const auto& targetPos = targetSp->GetPosition();
        const auto& center = m_view.getCenter();
        if (m_springiness != 0.f) {
            m_view.setCenter(lerp(targetPos, center,
                                  dt * 0.000025 * (1.f / m_springiness)));
        } else {
            m_view.setCenter(targetPos);
        }
    }
    window.setView(m_view);
}

void Camera::SetCenter(const Vec2& center) {
    m_view.setCenter(center);
}

void Camera::SetZoom(float zoom, const UIVec2& windowSize) {
    m_view.setSize({ windowSize.x * (1.f / zoom),
                     windowSize.y * (1.f / zoom) });
}

FloatRect Camera::GetBounds() const {
    const auto& center = m_view.getCenter();
    const auto& size = m_view.getSize();
    return {
        { center.x - size.x / 2.f, center.y - size.y / 2.f },
        size
    };
}

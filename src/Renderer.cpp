#include "Renderer.hpp"
#include "GraphicsComponent.hpp"
#include "Animation.hpp"
#include "Entity.hpp"
#include "Camera.hpp"
#include "Utility.hpp"

void Renderer::Configure(const Camera& camera) {
    m_config.cameraViewBounds = camera.GetBounds();
    m_config.cameraViewCenter = camera.GetView().getCenter();
    m_config.cameraViewSize = camera.GetView().getSize();
}

void Renderer::Visit(Entity& entity, AnimationComponent& comp) {
    auto keyframe = comp.GetAnimation()->GetKeyframe(comp.GetKeyframe());
    if (entity.HasAttribute(SGE_Attr_PositionAbsolute)) {
        keyframe.setPosition(AbsoluteTransform(m_config.cameraViewCenter,
                                               m_config.cameraViewSize,
                                               entity.GetPosition()));
    } else {
        keyframe.setPosition(entity.GetPosition());
    }
    keyframe.setScale(comp.GetScale());
    if (m_config.cameraViewBounds.intersects(keyframe.getGlobalBounds())) {
        keyframe.setColor(comp.GetColor());
        m_displayList.push_back({
            keyframe, comp.GetRenderStates(), comp.GetZOrder()
        });
    }
}

void Renderer::Display(sf::RenderWindow& window) {
    std::sort(m_displayList.begin(), m_displayList.end(),
              [](const RenderTask& lhs, const RenderTask& rhs) {
                  return lhs.zOrder < rhs.zOrder;
              });
    for (const auto& element : m_displayList) {
        window.draw(element.sprite, element.renderStates);
    }
    m_displayList.clear();
}

#include "RendererImpl.hpp"

RendererImpl::RendererImpl(sf::RenderWindow& target) :
    m_target(target) {}

void RendererImpl::Visit(const Wall& wall) {
    // ...
}

void RendererImpl::Run() {
    // ...
}

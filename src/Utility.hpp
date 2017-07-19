#pragma once

#include <utility>
#include <memory>

#include "Types.hpp"

Vec2 AbsoluteTransform(const sf::View& view, const Vec2& pos);

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

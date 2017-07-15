#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>

using Vec2 = sf::Vector2<float>;
using IVec2 = sf::Vector2<int>;
using UIVec2 = sf::Vector2<unsigned>;
using Color = sf::Color;
using RenderStates = sf::RenderStates;
using Rect = sf::IntRect;
using USec = std::chrono::microseconds::rep;
using UID = size_t;
using ZOrderIndex = int;

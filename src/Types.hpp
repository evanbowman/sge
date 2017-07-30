#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <chrono>
#include <bitset>

#include "../include/SGE/SGE_Types.h"

using Vec2 = sf::Vector2<float>;
using IVec2 = sf::Vector2<int>;
using UIVec2 = sf::Vector2<unsigned>;
using IntRect = sf::Rect<int>;
using FloatRect = sf::Rect<float>;
using Color = sf::Color;
using RenderStates = sf::RenderStates;
using String = std::string;
using USec = std::chrono::microseconds::rep;
using ZOrderIndex = int;
class Entity;
using EntityRef = std::shared_ptr<Entity>;
using WeakEntityRef = std::weak_ptr<Entity>;
using AttributeSet = std::bitset<SGE_Attr_Count>;

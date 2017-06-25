#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

class TextureManager {
public:
    sf::Texture& GetTexture(const std::string& fileName);
    
private:
    std::unordered_map<std::string, sf::Texture> m_textures;
};

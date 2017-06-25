#include "TextureManager.hpp"
#include "ResourcePath.hpp"
#include "Exceptions.hpp"

sf::Texture& TextureManager::GetTexture(const std::string& fileName) {
    auto texture = m_textures.find(fileName);
    if (texture == m_textures.end()) {
        auto& texture = (m_textures[fileName] = sf::Texture{});
        if (!texture.loadFromFile(ResourcePath() + "textures/" + fileName)) {
            m_textures.erase(fileName);
            throw MissingResource(fileName);
        }
        return texture;
    }
    return texture->second;
}

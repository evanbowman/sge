#include "TextureManager.hpp"
#include "ResourcePath.hpp"
#include "Exceptions.hpp"

TextureLoadRequest::TextureLoadRequest(const std::string& fileName) :
    m_fileName(fileName), m_result(nullptr) {}

sf::Texture* TextureLoadRequest::GetResult() {
    while (true) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_result) {
            break;
        }
    }
    return m_result;
}

void TextureManager::Accept(TextureLoadRequest& req) {
    std::lock_guard<std::mutex> lk(req.m_mutex);
    req.m_result = &GetTexture(req.m_fileName);
}

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

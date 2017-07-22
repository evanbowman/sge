#include "TextureManager.hpp"

TextureRequest::TextureRequest(const std::string& fileName) :
    m_fileName(fileName), m_result(nullptr) {}

sf::Texture* TextureRequest::GetResult() {
    while (true) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_result) {
            break;
        }
    }
    return m_result;
}

void TextureManager::Accept(TextureRequest& req) {
    std::lock_guard<std::mutex> lk(req.m_mutex);
    req.m_result = &LoadTexture(req.m_fileName);
}

sf::Texture& TextureManager::LoadTexture(const std::string& path) {
    auto texture = m_textures.find(path);
    if (texture == m_textures.end()) {
        auto& texture = (m_textures[path] = sf::Texture{});
        if (!texture.loadFromFile(path)) {
            m_textures.erase(path);
            throw std::runtime_error("Missing texture");
        }
        return texture;
    }
    return texture->second;
}

#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

class TextureRequest {
public:
    explicit TextureRequest(const std::string& fileName);
    friend class TextureManager;
    sf::Texture* GetResult();
private:
    std::string m_fileName;
    sf::Texture* m_result;
    std::mutex m_mutex;
};

class TextureManager {
public:
    void Accept(TextureRequest& req);
    
private:
    sf::Texture& LoadTexture(const std::string& fileName);
    std::unordered_map<std::string, sf::Texture> m_textures;
};

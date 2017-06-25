#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

class TextureLoadRequest {
public:
    explicit TextureLoadRequest(const std::string& fileName);
    friend class TextureManager;
    sf::Texture* GetResult();
private:
    std::string m_fileName;
    sf::Texture* m_result;
    std::mutex m_mutex;
};

class TextureManager {
public:
    void Accept(TextureLoadRequest& req);
    
private:
    sf::Texture& GetTexture(const std::string& fileName);
    std::unordered_map<std::string, sf::Texture> m_textures;
};

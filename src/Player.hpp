#pragma once

#include "Hero.hpp"

class Player {
public:
    void SetCharacter(std::shared_ptr<Hero> character);
    Hero* GetCharacter();
    std::shared_ptr<Hero> GetCharacterRef();
    
private:
    std::shared_ptr<Hero> m_character;
};

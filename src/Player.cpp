#include "Player.hpp"

Hero* Player::GetCharacter() {
    return m_character.get();
}

void Player::SetCharacter(std::shared_ptr<Hero> character) {
    m_character = character;
}

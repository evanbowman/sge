#pragma once

class Game;

class GameBuilder {
public:
    Game Build(GameBuilder && builder);
};

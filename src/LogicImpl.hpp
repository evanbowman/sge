#pragma once

#include "Logic.hpp"

class LogicImpl : public Logic {
public:
    void Visit(const Wall& wall) override;
};

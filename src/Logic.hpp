#pragma once

#include "Types.hpp"

class Wall;
class Hero;

// Entities don't implement their update logic
// internally, instead delegating that responsibility
// to a Logic visitor object. This architecture makes
// it easy to add new implementations for game logic,
// for example, off-loading the logic to Lua or Lisp.

class Logic {
public:
    virtual ~Logic() {}
    virtual void Visit(const Wall& wall) = 0;
    
    void SetDT(USec dt) {
        m_dt = dt;
    }
protected:
    USec m_dt;
};

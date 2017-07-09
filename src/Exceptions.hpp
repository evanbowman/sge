#pragma once

#include <stdexcept>
#include <string>

#include "Types.hpp"

class MissingResource : public std::runtime_error {
public:
    MissingResource(const std::string& resourceName);
};

class BadHandle : public std::exception {
public:
    explicit BadHandle(UID handle);
    
    const char* what() const noexcept override;

    UID which() const;
    
private:
    UID m_badId;
};

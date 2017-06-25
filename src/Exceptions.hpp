#pragma once

#include <stdexcept>
#include <string>

class MissingResource : public std::runtime_error {
public:
    MissingResource(const std::string& resourceName);
};

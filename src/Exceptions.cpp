#include "Exceptions.hpp"

MissingResource::MissingResource(const std::string& resourceName) :
    std::runtime_error("Could not find or load resource " + resourceName) {}

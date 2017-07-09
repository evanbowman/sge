#include "Exceptions.hpp"

MissingResource::MissingResource(const std::string& resourceName) :
    std::runtime_error("Could not find or load resource " + resourceName) {}

BadHandle::BadHandle(UID handle) : m_badId(handle) {}

const char* BadHandle::what() const noexcept {
    return ("Bad handle, UID: " + std::to_string(m_badId)).c_str();
}

UID BadHandle::which() const {
    return m_badId;
}

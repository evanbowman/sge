#pragma once

#include <unordered_map>

#include "PoolAllocator.hpp"

template <typename K, typename T>
class PooledHashmap : public std::unordered_map<K,
                                                T,
                                                std::hash<K>,
                                                std::equal_to<K>,
                                                PoolAllocator<std::pair<const K,
                                                                        T>>> {};

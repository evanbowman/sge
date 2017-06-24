#pragma once

#include <array>

template <typename T, size_t Rows, size_t Cols>
struct Matrix :
    public std::array<std::array<T, Rows>, Cols> {};

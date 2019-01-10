#pragma once

#include <cstddef>

#define DISALLOW_COPY_AND_ASSIGN(TypeName)    \
    TypeName(const TypeName&) = delete;       \
    void operator=(const TypeName&) = delete

#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName)  \
    TypeName() = delete;                          \
    DISALLOW_COPY_AND_ASSIGN(TypeName)

template <typename T, size_t N>
char (&ArraySizeHelper(const T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

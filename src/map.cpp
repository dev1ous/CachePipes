// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

// This file uses Boost.Unordered
// Licensed under the Boost Software License 1.0
// See THIRD_PARTY_LICENSES.md for full license text

// This file includes code from rapidhash - Very fast, high quality, platform independant hashing algorithm.
// Copyright (C) 2024 Nicolas De Carli
// Licensed under BSD 2-Clause License
// See THIRD_PARTY_LICENSES.md for full license text

#include "map.hpp"

#include <rapidhash.h>


uint64_t dev1::detail::ihash::operator()(char const * str) const noexcept
{
    return rapidhash(str, strlen(str));
}

uint64_t dev1::detail::ihash::operator()(std::string_view str) const noexcept
{
    return rapidhash(str.data(), str.size());
}

uint64_t dev1::detail::ihash::hash_bytes(void const *data, std::size_t len) noexcept
{
    return rapidhash(data, len);
}

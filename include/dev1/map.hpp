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

#ifndef MAP_HPP
#define MAP_HPP

#include <type_traits>

#include <boost/unordered/unordered_flat_map.hpp>

namespace dev1::detail {
    struct ihash {
        using is_transparent = void;

        uint64_t operator()(char const* str) const noexcept;
        uint64_t operator()(std::string_view str) const noexcept;

        static uint64_t hash_bytes(void const* data, std::size_t len) noexcept;

        template<typename T>
        requires std::is_trivially_copyable_v<T>
        uint64_t operator()(T const& val) const noexcept {
            return hash_bytes(&val, sizeof(T));
        }
    };
}

namespace boost::unordered {
    template <>
    struct hash_is_avalanching<dev1::detail::ihash> {
        static constexpr bool value = true;
    };
}

namespace dev1 {
    namespace detail {
        namespace unordered = boost::unordered;
    }

    template<typename Key, typename Value>
    using unordered_map = detail::unordered::unordered_flat_map<
        Key, 
        Value, 
        detail::ihash, 
        std::equal_to<>
    >;
}

#endif
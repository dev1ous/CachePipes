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

#ifndef PROTOTYPE_MAP_HPP
#define PROTOTYPE_MAP_HPP

#include <rapidhash.h>
#include <boost/unordered/unordered_node_map.hpp>

#include "type_erasure.hpp"

namespace dev1::detail {
    struct ihash {
        using is_transparent = void;

        uint64_t operator()(char const* str) const noexcept {
            return rapidhash(str, strlen(str));
        }
        uint64_t operator()(std::string_view str) const noexcept {
            return rapidhash(str.data(), str.size());
        }
        uint64_t operator()(std::string const& str) const noexcept {
            return rapidhash(str.data(), str.size());
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

        class unordered_map : public unordered::unordered_node_map<
            std::string, 
            std::weak_ptr<concept_t>, 
            detail::ihash, 
            std::equal_to<>
        > {};
    }
}

#endif
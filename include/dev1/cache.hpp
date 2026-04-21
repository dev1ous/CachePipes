// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#ifndef CACHE_HPP
#define CACHE_HPP

#include "detail/prototype_map.hpp"

namespace dev1 {

    class Cache
    {
    public: 
        Cache() = default;
        ~Cache() = default;
        Cache(Cache&&) = default;
        Cache& operator=(Cache&&) = default;

        operator detail::unordered_map *() {
            return &cache_map;
        }

        std::size_t get_size() { return cache_map.size(); }
    private:
        detail::unordered_map cache_map;

        Cache(Cache const&) = delete;
        Cache& operator=(Cache const&) = delete;
    };
}
  
#endif

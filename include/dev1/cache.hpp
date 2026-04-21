// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#ifndef CACHE_HPP
#define CACHE_HPP

#include "detail/cache_map.hpp"

namespace dev1 {
    class Cache
    {
    public: 
        Cache() = default;
        ~Cache() = default;
        Cache(Cache&&) = default;
        Cache& operator=(Cache&&) = default;

        operator detail::cache_map *() {
            return &cache;
        }

        std::size_t get_size() { return cache.size(); }
    private:
        detail::cache_map cache;

        Cache(Cache const&) = delete;
        Cache& operator=(Cache const&) = delete;
    };
}
  
#endif

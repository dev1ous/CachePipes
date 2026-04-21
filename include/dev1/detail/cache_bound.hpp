#ifndef CACHE_BOUND_HPP
#define CACHE_BOUND_HPP

#include "cache_map.hpp"

namespace dev1::detail {
    template<typename Stage>
    struct cache_bound {
        cache_map& cache;
        Stage stage;

        template<typename Proxy>
        [[nodiscard]] auto operator|(Proxy proxy) && {
            return cache_bound<decltype(std::move(stage) | std::move(proxy))>{
                cache, std::move(stage) | std::move(proxy)
            };
        }

        [[nodiscard]] auto operator()() {
            return std::move(stage)(cache);
        }
    };
}
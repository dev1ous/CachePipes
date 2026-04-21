// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#ifndef PRODUCE_HPP
#define PRODUCE_HPP

#include <memory>
#include <type_traits>
#include "detail/cache_bound.hpp"
#include "detail/type_erasure.hpp"
#include "detail/produce_proxy.hpp"

namespace dev1 {

    class Cache;

    namespace detail {
        /// @brief Extracts T from std::shared_ptr<model<T>>.
        // Error if the produce() lambda returns anything else.
        template<typename Ptr> 
        struct extract_model_type {
            static_assert(sizeof(Ptr) == 0,
                "produce() lambda must return std::shared_ptr<model<T>>. "
                "Use dev1::make<T>() inside the lambda for convenience.");
        };
        template<typename T>
        struct extract_model_type<std::shared_ptr<model<T>>> { 
            using type = T; 
        };

        template<typename Func, typename T>
        struct produce_proxy {
            using resource_type = T;
            
            [[no_unique_address]] Func func;
        };

        struct produce_t {
            template<typename Func>
            [[nodiscard]] auto operator()(Func&& func) const noexcept {
                using ReturnType = std::invoke_result_t<Func>;
                using T = typename detail::extract_model_type<ReturnType>::type;
                return produce_proxy<std::remove_cvref_t<Func>, T>{ std::forward<Func>(func) };
            }
        };

        template<typename Func, typename T>
        [[nodiscard]] auto operator|(cache_map* cache, produce_proxy<Func, T> pp) {
            return cache_bound<produce_proxy<Func, T>>{ cache, std::move(pp) };
        }
    }
}

namespace dev1 {

    inline constexpr detail::produce_t produce{};

}

#endif
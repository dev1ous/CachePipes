// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

// This file uses Boost.Hana by Louis Dionne
// Copyright Louis Dionne 2013-2022
// Licensed under the Boost Software License 1.0
// See THIRD_PARTY_LICENSES.md for full license text

#ifndef PRODUCE_HPP
#define PRODUCE_HPP

#include <boost/hana/lazy.hpp>
#include <boost/hana/tuple.hpp>

namespace dev1 {
    namespace detail {
        namespace hana = boost::hana;
        
        template<typename T> struct model;

        template <typename Ptr>
        struct ExtractModelType {
            static_assert(sizeof(Ptr) == 0,
                "Error: The Return Type need to be of type std::shared_ptr<model<T>>.");
        };

        template <typename T>
        struct ExtractModelType<std::shared_ptr<model<T>>> {
            using type = T;
        };

        struct produce_t {
            template<typename Func>
            [[nodiscard]] constexpr auto operator()(Func&& func) const noexcept
            { 
                using ReturnType = std::invoke_result_t<Func>;
                using T = typename ExtractModelType<ReturnType>::type;

                return hana::make_lazy([func = std::forward<Func>(func)] (auto& cacheRef) {
                    return hana::make_tuple(func, cacheRef, hana::type_c<T>);
                });
            }
        };
    }

    inline constexpr detail::produce_t produce{};
}

#endif /* PRODUCE_HPP */
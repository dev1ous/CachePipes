// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#ifndef MAKE_HPP
#define MAKE_HPP

#include "detail/type_erasure.hpp"

namespace dev1 {
    namespace detail {
        template<typename T>
        struct make_t {
            template<typename ...Args>
            [[nodiscard]] std::shared_ptr<model<T>> operator()(Args... args) const {
                return std::make_shared<model<T>>(std::forward<Args>(args)...);
            }
        };
    }
    template<typename T>
    inline constexpr detail::make_t<T> make{};
}

#endif
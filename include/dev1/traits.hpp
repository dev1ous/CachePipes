// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#ifndef TRAITS_HPP
#define TRAITS_HPP

#include "detail/concept.hpp"

using namespace std::literals;

namespace dev1 {
    //implementation of the functions non meaningful, just here to fill
    template<typename T>
    struct traits {
        static constexpr auto extensions() {
            return std::vector<std::string_view>{};
        }

        static constexpr auto loader(T& resource, std::filesystem::path const& path) {
            return false; 
        }
    };

    namespace detail {
        template<typename T>
        struct validator {
            template<typename U = T>
            static constexpr auto extensions() {
                return traits<U>::extensions();
            }

            template<typename U = T>
            static constexpr auto get_size() {
                return std::size(extensions<U>());
            }

            static constexpr auto loader(T& resource, std::filesystem::path const& path) {
                return traits<T>::loader(resource, path);
            }

            template<typename U = T>
            static constexpr bool is_valid_extensions = requires(U t) {
                { get_size<U>() } -> std::convertible_to<size_t>;
                requires impl::ValidExtensions<traits<U>, get_size<U>()>;
            };

            template<typename U = T>
            static constexpr bool is_valid_loader = impl::ValidLoader<traits<U>, U>;
        };
    }
}

#endif
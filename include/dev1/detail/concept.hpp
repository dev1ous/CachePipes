// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

// This file uses Boost.Hana by Louis Dionne
// Copyright Louis Dionne 2013-2022
// Licensed under the Boost Software License 1.0
// See THIRD_PARTY_LICENSES.md for full license text

#ifndef CONCEPT_HPP
#define CONCEPT_HPP

#include <filesystem>
#include <string_view>

#include <boost/hana/lazy.hpp>
#include <boost/hana/functional/partial.hpp>

namespace dev1 {
    namespace hana = boost::hana;

    namespace detail {
        namespace impl {
            template<typename Container, std::size_t N>
            concept StringViewContainer = 
                std::convertible_to<typename Container::value_type, std::string_view> &&
                (std::convertible_to<Container, std::array<std::string_view, N>> ||
                (std::is_array_v<Container> && 
                std::convertible_to<std::remove_extent_t<Container>, std::string_view>));

            template<typename Traits, std::size_t N>
            concept ValidExtensions = requires {
                { Traits::extensions() } -> StringViewContainer<N>;
            };

            template<typename Traits, typename T>
            concept ValidLoader = requires(T& resource, std::filesystem::path const& filepath) {
                { Traits::loader(resource, filepath) } -> std::same_as<bool>;
            } || requires(T& resource, std::string const& filepath) {
                { Traits::loader(resource, filepath) } -> std::same_as<bool>;
            };

            template<typename T>
            struct get_partial_params;

            template<typename Int, Int... n, typename F, typename... X>
            struct get_partial_params<hana::partial_t<std::integer_sequence<Int, n...>, F, X...>> {
                using function_type = F;
                template<std::size_t N>
                using param_type = std::tuple_element_t<N, std::tuple<X...>>;
            };

            template<typename T, typename Partial>
            concept FirstParamOfType = requires {
                requires std::same_as<
                    typename get_partial_params<Partial>::template param_type<0>, 
                    T
                >;
            };

            template <typename T>
            struct is_lazy_apply : std::false_type {};

            template <typename ...Args>
            struct is_lazy_apply<hana::lazy_apply_t<Args...>> : std::true_type {};

            template <typename T>
            concept LazyApply = is_lazy_apply<T>::value;

            template<typename T>
            struct is_shared_ptr : std::false_type {};

            template<typename T>
            struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

            template<typename T>
            concept IsSharedPtr = is_shared_ptr<T>::value;

            template<typename Handler, typename Resource>
            concept ValidResourceHandler = requires(Handler&& handler, Resource& resource, std::filesystem::path const& filepath) {
                { std::invoke(std::forward<Handler>(handler), resource, filepath) };
            } && (std::is_function_v<std::remove_pointer_t<std::remove_reference_t<Handler>>> ||
                std::is_class_v<std::remove_reference_t<Handler>>);
        }
    }
}

#endif /* CONCEPT_HPP */
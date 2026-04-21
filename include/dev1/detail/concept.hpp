// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#ifndef CONCEPT_HPP
#define CONCEPT_HPP

#include <array>
#include <concepts>
#include <filesystem>
#include <functional>
#include <string_view>
#include <type_traits>

namespace dev1::detail::impl {
    /// @brief extensions() must return a fixed-size array of string_view
    template<typename Container, std::size_t N>
    concept StringViewContainer =
        std::convertible_to<typename Container::value_type, std::string_view> &&
        std::convertible_to<Container, std::array<std::string_view, N>>;

    template<typename Traits, std::size_t N>
    concept ValidExtensions = requires {
        { Traits::extensions() } -> StringViewContainer<N>;
    };

    /// @brief loader() must take (T&, path or string) and return bool
    template<typename Traits, typename T>
    concept ValidLoader =
        requires(T& r, std::filesystem::path const& p) {
            { Traits::loader(r, p) } -> std::same_as<bool>;
        } ||
        requires(T& r, std::string const& s) {
            { Traits::loader(r, s) } -> std::same_as<bool>;
        };

    /// @brief A user supplied loader function/lambda/functor for load_resource(fn)
    template<typename Handler, typename Resource>
    concept ValidResourceHandler =
        requires(Handler&& h, Resource& r, std::filesystem::path const& p) {
            { std::invoke(std::forward<Handler>(h), r, p) };
        } &&
        (std::is_function_v<std::remove_pointer_t<std::remove_reference_t<Handler>>> ||
         std::is_class_v<std::remove_reference_t<Handler>>);

    template<typename T>
    concept PipelineStage = requires {
        typename T::resource_type;
        typename T::producer_type;
    };
}

#endif // CONCEPT_HPP
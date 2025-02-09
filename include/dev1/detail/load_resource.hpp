// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

// This file uses Boost.Hana by Louis Dionne
// Copyright Louis Dionne 2013-2022
// Licensed under the Boost Software License 1.0
// See THIRD_PARTY_LICENSES.md for full license text

// This file uses swl-variant by Jean-Baptiste Vallon Hoarau
// Copyright (c) 2021 Jean-Baptiste Vallon Hoarau
// Licensed under MIT License
// See THIRD_PARTY_LICENSES.md for full license text

#ifndef LOAD_RESOURCE_HPP
#define LOAD_RESOURCE_HPP

#include <swl/variant.hpp>
#include <boost/hana/drop_back.hpp>

#include "tags.hpp"
#include "traits.hpp"
#include "hana_utilities.hpp"

namespace dev1 {
    namespace detail {
        namespace hana = boost::hana;

        struct empty_t{};

        struct continuation_load_resource_t {
            template<typename GetVariant>
            using load_resource_variant = swl::variant<
                typename swl::variant_alternative_t<0, std::remove_cvref_t<GetVariant>>,
                hana::tuple<
                    typename std::tuple_element_t<0, typename swl::variant_alternative_t<1, std::remove_cvref_t<GetVariant>>>,
                    typename std::tuple_element_t<1, typename swl::variant_alternative_t<1, std::remove_cvref_t<GetVariant>>>,
                    typename std::tuple_element_t<2, typename swl::variant_alternative_t<1, std::remove_cvref_t<GetVariant>>>
                >
            >;

            template<typename TupleRef>
            using load_resource_tuple = hana::tuple<
                std::remove_cvref_t<decltype(hana::at_c<0>(std::declval<TupleRef>()))>,
                std::remove_cvref_t<decltype(hana::at_c<1>(std::declval<TupleRef>()))>,
                std::remove_cvref_t<decltype(hana::at_c<2>(std::declval<TupleRef>()))>
            >;

            template<typename Func, typename TupleRef>
            auto operator()([[maybe_unused]] full, [[maybe_unused]] Func func, TupleRef const& tupleRef) const 
                -> load_resource_tuple<TupleRef>
            {
                auto const& shrdPtr = hana::at_c<0>(tupleRef);
                [[maybe_unused]] auto name = hana::at_c<2>(tupleRef);
                auto const& typeWrapper = hana::at_c<3>(tupleRef);
                auto const& filepath = hana::at_c<4>(tupleRef);

                auto& resource = *shrdPtr;

                using ResourceType = typename std::remove_cvref_t<decltype(typeWrapper)>::type;

                if constexpr (std::is_same_v<std::remove_cvref_t<Func>, empty_t>) {
                    static_assert(validator<ResourceType>::template is_valid_loader<ResourceType>, 
                        "You need to use a boolean as return type, a reference to Your Resource as first parameter and a const reference of a std::filesystem::path or std::string for your Resource traits specialization");

                    if constexpr (validator<ResourceType>::template is_valid_loader<ResourceType>) {
                        if (!validator<ResourceType>::loader(resource, filepath)) {
                            std::string errorMessage = "Failed to load : ";
                            errorMessage.append(name);
                            throw std::runtime_error(errorMessage);
                        }
                    }
                } else {
                    static_assert(impl::ValidResourceHandler<Func, ResourceType>,
                        "Your function must be a function pointer, lambda, or functor class that accepts "
                        "(YourResource&, std::filesystem::path const&) as parameters");

                    if (!func(resource, filepath)) {
                        std::string errorMessage = "Failed to load : ";
                        errorMessage.append(name);
                        throw std::runtime_error(errorMessage);
                    }
                }
                return hana::drop_back(tupleRef, hana::size_c<2>);
            }

            template<typename Func, typename Variant>
            auto operator()([[maybe_unused]] safety, [[maybe_unused]] Func func, Variant const& variant) const 
                -> load_resource_variant<Variant>
            {
                if (variant.index() == 0) {
                    return swl::unsafe_get<0>(variant);
                }
                auto const& tupleRef = swl::unsafe_get<1>(variant);
                return this->operator()(full{}, func, tupleRef);
            }
        };

        struct load_resource_t {
            template<typename Func = empty_t>
            struct proxy {
                proxy() = default;
                constexpr explicit proxy(Func&& func): _func(std::forward<Func>(func)) {}

                proxy(proxy const& other) = default;
                proxy& operator=(proxy const& other) = default;
                
                proxy(proxy&& other) = default;
                proxy& operator=(proxy&& other) = default;
                
                ~proxy() = default;

                template<typename Tag>
                constexpr auto dispatch() const& {
                    return hana::partial(continuation_load_resource_t{}, Tag{}, _func);
                }

                [[no_unique_address]] Func _func;
            };

            template<typename Func>
            [[nodiscard]] constexpr auto operator()(Func&& func) const noexcept {
                return proxy{std::forward<Func>(func)};
            }

            [[nodiscard]] constexpr auto operator()() const noexcept {
                return proxy{};
            }
        };
    }

    inline constexpr detail::load_resource_t load_resource{};
}

#endif /* LOAD_RESOURCE_HPP */
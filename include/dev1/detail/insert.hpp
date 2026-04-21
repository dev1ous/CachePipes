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

#ifndef INSERT_HPP
#define INSERT_HPP

#include <utility>
#include <stdexcept>

#include <boost/hana/functional/partial.hpp>

#include "tags.hpp"
#include "variant_utilities.hpp"

namespace dev1 {
    namespace detail {
        namespace hana = boost::hana;

        struct continuation_insert_t {
            template<typename TupleRef>
            using shared_ptr_t = std::remove_cvref_t<decltype(hana::at_c<0>(std::declval<TupleRef>()))>;

            template<typename TupleRef>
            auto operator()([[maybe_unused]] full, TupleRef const& tupleRef) const 
                -> shared_ptr_t<TupleRef>
            { 
                auto shrdPtr = hana::at_c<0>(tupleRef);
                auto& cache = hana::at_c<1>(tupleRef);
                auto name = hana::at_c<2>(tupleRef);

                if (!cache->insert(std::make_pair(std::string(name), shrdPtr)).second) {
                    throw std::runtime_error("You already have inserted an element with the same key");
                }
                return shrdPtr;
            }
            template<typename Variant>
            auto operator()([[maybe_unused]] safety, Variant const& variant) const 
            { 
                if (variant.index() == 0) {
                    return get_unbound<0>(variant);
                }
                auto const& tupleRef = get_unbound<1>(variant);
                return this->operator()(full{}, tupleRef);
            }
        };

        struct insert_t {
            struct proxy {
                template<typename Tag>
                constexpr auto dispatch() const& {
                    return hana::partial(continuation_insert_t{}, Tag{});
                }
            };
            [[nodiscard]] constexpr auto operator()() const noexcept {
                return proxy{};
            }
        };
    }

    inline constexpr detail::insert_t insert{};
}

#endif /* INSERT_HPP */
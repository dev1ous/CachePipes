// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

// This file uses Boost.Hana by Louis Dionne
// Copyright Louis Dionne 2013-2022
// Licensed under the Boost Software License 1.0
// See THIRD_PARTY_LICENSES.md for full license text

// tuple_element for hana::tuple has been taken from an answer here : https://github.com/boostorg/hana/issues/317

#ifndef HANA_UTILITIES_HPP
#define HANA_UTILITIES_HPP

#include <boost/hana/tuple.hpp>
#include <boost/hana/integral_constant.hpp>

namespace std {
    template<std::size_t n, typename... Types>
    struct tuple_element<n, boost::hana::tuple<Types...>> {
        using type = typename decltype(+boost::hana::tuple_t<Types...>[boost::hana::size_c<n>])::type;
    };
}

#endif

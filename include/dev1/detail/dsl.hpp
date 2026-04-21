// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

// This file uses Boost.Hana by Louis Dionne
// Copyright Louis Dionne 2013-2022
// Licensed under the Boost Software License 1.0
// See THIRD_PARTY_LICENSES.md for full license text

// Final Design inspired from Louis Dionne "Fun with Boost Hana" 2017 talk at Cppnow

#ifndef DSL_HPP
#define DSL_HPP

#include <boost/hana/map.hpp>

namespace dev1 {
    namespace detail {
        namespace hana = boost::hana;
        
        template<typename... Res>
        struct resource_collection {
            static constexpr std::size_t size = sizeof...(Res);
            static constexpr bool has_multiple = (sizeof...(Res) > 1);
        };

        template<typename... Groups>
        struct resource_path_map {
        private:
            static constexpr auto all_pairs = hana::chain(
                hana::make_basic_tuple(Groups{}...),
                [](auto group) { return group.pairs; }
            );
            static constexpr auto map = hana::unpack(all_pairs, hana::make_map);

        public:
            template<typename Path>
            inline constexpr auto get_resources(Path path) const {
                return map[path];
            }
        };

        template<typename... Res>
        struct resource_wrapper {
            template <typename... Paths>
            inline constexpr auto operator=(Paths... ps) const {
                struct group {
                    static constexpr auto pairs = hana::make_basic_tuple(
                        hana::make_pair(ps, resource_collection<Res...>{})...
                    );
                };
                return group{};
            }
        };
    }

    template<typename... Res>
    inline constexpr detail::resource_wrapper<Res...> Resources{};

    template<typename Res>
    inline constexpr auto Resource = Resources<Res>;

    template<typename... Groups>
    inline constexpr auto make_resource_map(Groups... groups) {
        return detail::resource_path_map<Groups...>{groups...};
    }
}

#endif
// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

// This file uses Boost.Hana by Louis Dionne
// Copyright Louis Dionne 2013-2022
// Licensed under the Boost Software License 1.0
// See THIRD_PARTY_LICENSES.md for full license text

#ifndef LOAD_HPP
#define LOAD_HPP

#include <thread>
#include <memory>

#include <boost/hana/for_each.hpp>

#include "dsl.hpp"
#include "traits.hpp"
#include "resource_map.hpp"

namespace dev1 {
    namespace detail {
        namespace hana = boost::hana;

        template<typename Map1, typename Map2>
        constexpr auto transform_maps(Map1 const& map1,  Map2 const& map2) {
            return hana::make_map(
                hana::to_tuple(map1)
                | hana::filter([&map2](auto pair1) {
                    auto key1 = hana::first(pair1);
                    return hana::contains(map2, key1);
                })
                | hana::chain([&map2](auto pair1) {
                    auto key1 = hana::first(pair1);
                    auto value1 = hana::second(pair1);

                    if constexpr (decltype(value1)::has_multiple) {
                        if constexpr (decltype(map2[key1])::has_multiple) {
                            return hana::make_tuple(hana::make_pair(map2[key1], value1));
                        }
                        else {
                            static_assert(false, "You can't have multiple Resource Type when only one path for your resource name has been found");
                            return hana::make_tuple();
                        }
                    }
                    else {
                        auto tuple_map = map2[key1].to_tuple();
                        auto first_elem = hana::at_c<0>(value1.to_tuple());

                        if constexpr (decltype(map2[key1])::has_multiple) {
                            return tuple_map
                                | hana::transform([value1](auto x) {
                                    return hana::make_pair(x, first_elem);
                                });
                        } else {
                            return hana::make_tuple(
                                hana::make_pair(hana::at_c<0>(tuple_map), first_elem);
                            );
                        }
                    }
                })
            );
        }

        struct load_t {
            template<typename... Groups>
            [[nodiscard]] constexpr auto operator()(std::string_view entry_dir, detail::resource_path_map<Groups...> const& map) const {
#ifdef RESOURCE_MAP_PAIRS
                constexpr auto resource_map = ResourceMap::get_map(); 
                return hana::if_(hana::keys(map) ^hana::contains^ hana::front(hana::keys(resource_map)), 
                    transform_maps(map, resource_map),
                    map
                );
#else
                std::vector<std::filesystem::path> paths{};
                std::filesystem::path entry_path{entry_dir};

                hana::for_each(map._paths, [&entry_path, &paths] (auto pair) {
                    using ResourceType = typename decltype(+hana::second(pair))::type;
                    constexpr auto array = validator<ResourceType>::extensions();
                    constexpr std::string_view name = hana::first(pair).to_string_view();

                    for (auto const& entry : std::filesystem::recursive_directory_iterator(entry_path, std::filesystem::directory_options::skip_permission_denied)) {
                        std::filesystem::path path = entry.path();

                        if (path.stem() == name)
                        {
                            for (size_t i = 0; i < array.size(); ++i)
                            {
                                if (path.extension() == array[i])
                                {
                                    paths.push_back(std::move(path));
                                }
                            }
                        }
                    }
                });
#endif

                size_t const thread_count = std::thread::hardware_concurrency();
                size_t const batch_size = std::max(size_t(1), paths.size() / thread_count);

                std::vector<std::thread> workers;
                for (size_t i = 0; i < paths.size(); i += batch_size) {
                    size_t end = std::min(i + batch_size, paths.size());
                    workers.emplace_back([&paths, start=i, end] {
                        for (size_t j = start; j < end; ++j) {
                            auto texture = std::make_shared<sf::Texture>();
                            if(validator::loader<ResourceType>(paths[j].string())) {
                                image_map_.try_emplace_or_visit(
                                    image_paths[j].filename().string(),
                                    texture,
                                    [](auto& x) { /* ... */ }
                                );
                            }
                        }
                    });
                }
            }
        };
    }
    inline constexpr detail::load_t load{};
}

#endif
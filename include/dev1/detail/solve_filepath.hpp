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

#ifndef SOLVE_FILEPATH_HPP
#define SOLVE_FILEPATH_HPP

#include <swl/variant.hpp>
#include <boost/hana/append.hpp>

#include "tags.hpp"
#include "traits.hpp"
#include "hana_utilities.hpp"

namespace dev1 {
    namespace detail {
        namespace hana = boost::hana;

        struct solve_filepath_impl_t {
            template<typename GetVariant>
            using solve_filepath_variant = swl::variant<
                typename swl::variant_alternative_t<0, std::remove_cvref_t<GetVariant>>,
                hana::tuple<
                    typename std::tuple_element_t<0, typename swl::variant_alternative_t<1, std::remove_cvref_t<GetVariant>>>,
                    typename std::tuple_element_t<1, typename swl::variant_alternative_t<1, std::remove_cvref_t<GetVariant>>>,
                    typename std::tuple_element_t<2, typename swl::variant_alternative_t<1, std::remove_cvref_t<GetVariant>>>,
                    typename std::tuple_element_t<3, typename swl::variant_alternative_t<1, std::remove_cvref_t<GetVariant>>>,
                    std::filesystem::path
                >
            >;

            template<typename TupleRef>
            using solve_filepath_tuple = hana::tuple<
                std::remove_cvref_t<decltype(hana::at_c<0>(std::declval<TupleRef>()))>,
                std::remove_cvref_t<decltype(hana::at_c<1>(std::declval<TupleRef>()))>,
                std::remove_cvref_t<decltype(hana::at_c<2>(std::declval<TupleRef>()))>,
                std::remove_cvref_t<decltype(hana::at_c<3>(std::declval<TupleRef>()))>,
                std::filesystem::path
            >;

            template<typename T>
            [[nodiscard]] std::filesystem::path check_path(std::string_view base_file, std::string_view name) const {
                static_assert(validator<T>::template is_valid_extensions<T>, 
                    "You need to use a fixed size array of string_view as return type of your Resource traits specialization");

                if constexpr (validator<T>::template is_valid_extensions<T>) 
                {
                    constexpr auto array = validator<T>::extensions();
                    std::filesystem::path exec_dir = EXECUTABLE_DIR;

                    for (auto const& entry : std::filesystem::recursive_directory_iterator(exec_dir / base_file))
                    {
                        std::filesystem::path const& path = entry.path();

                        if (path.stem().string() == name)
                        {
                            for (size_t i = 0; i < array.size(); ++i)
                            {
                                if (path.extension().string() == array[i])
                                {
                                    return path;
                                }
                            }
                        }
                    }
                    return std::filesystem::path();
                }
            }

            template<typename TupleRef>
            auto operator()([[maybe_unused]] full, std::string_view basePath, TupleRef const& tupleRef) const 
                -> solve_filepath_tuple<TupleRef>
            {
                auto name = hana::at_c<2>(tupleRef);
                using T = typename std::remove_cvref_t<decltype(hana::at_c<3>(tupleRef))>::type;

                std::filesystem::path filepath = check_path<T>(basePath, name);

                if (filepath.empty()) {
                    std::string errorMessage = "Failed to find : ";
                    errorMessage.append(name);
                    throw std::runtime_error(errorMessage);
                }
                return hana::append(tupleRef, std::move(filepath));
            }

            template<typename Variant>
            auto operator()([[maybe_unused]] safety, std::string_view basePath, Variant const& variant) const
                -> solve_filepath_variant<Variant>
            {
                if (variant.index() == 0) {
                    return swl::unsafe_get<0>(variant);
                }
                auto const& tupleRef = swl::unsafe_get<1>(variant);
                return this->operator()(full{}, basePath, tupleRef);
            }
        };

        struct solve_filepath_t {
            struct proxy {
                proxy() = default;
                constexpr explicit proxy(std::string_view path) : basePath(path) {}
                
                proxy(proxy const& other) = default;
                proxy& operator=(proxy const& other) = default;
                
                proxy(proxy&& other) = default;
                proxy& operator=(proxy&& other) = default;
                
                ~proxy() = default;

                template<typename Tag>
                constexpr auto dispatch() const& {
                    return hana::partial(solve_filepath_impl_t{}, Tag{}, basePath);
                }
            private:
                std::string_view basePath;
            };
            [[nodiscard]] constexpr auto operator()(std::string_view basePath) const noexcept {
                return proxy{basePath};
            }
        };
    }
    inline constexpr detail::solve_filepath_t solve_filepath{};
}

#endif /* SOLVE_FILEPATH_HPP */
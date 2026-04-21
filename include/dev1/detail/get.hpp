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

#ifndef GET_HPP
#define GET_HPP

#include <boost/hana/functional/partial.hpp>

#include "tags.hpp"
#include "type_erasure.hpp"
#include "path_construct.hpp"

namespace dev1 {
    namespace detail {
        namespace hana = boost::hana;

        struct continuation_get_t {
            template<typename TupleRef>
            using producer_t = std::invoke_result_t<std::remove_cvref_t<decltype(hana::at_c<0>(std::declval<TupleRef>()))>>;

            template<typename TupleRef>
            using cache_t = std::remove_cvref_t<decltype(hana::at_c<1>(std::declval<TupleRef>()))>;

            template<typename TupleRef>
            using type_wrapper_t = std::remove_cvref_t<decltype(hana::at_c<2>(std::declval<TupleRef>()))>;

            template<typename TupleRef>
            using get_variant = swl::variant<
                producer_t<TupleRef>,
                hana::tuple<
                    producer_t<TupleRef>,
                    cache_t<TupleRef>,
                    std::string_view,
                    type_wrapper_t<TupleRef>
                >
            >;

            template<typename TupleRef>
            using get_tuple = hana::tuple<
                producer_t<TupleRef>,
                cache_t<TupleRef>,
                std::string_view,
                type_wrapper_t<TupleRef>
            >;

            template<typename T>
            [[nodiscard]] std::shared_ptr<model<T>> cast(std::shared_ptr<concept_t> const& basePtr) const {
                model<T>* derivedRawPtr = static_cast<model<T>*>(basePtr.get());
                return std::shared_ptr<model<T>>(basePtr, derivedRawPtr);
            }

            template<typename TupleRef>
            auto operator()([[maybe_unused]] perf, std::string_view name, TupleRef const& tupleRef) const 
                -> producer_t<TupleRef>
            {
                auto const& producer = hana::at_c<0>(tupleRef);
                auto& cache = hana::at_c<1>(tupleRef);
                auto typeWrapper = hana::at_c<2>(tupleRef);

                using T = typename std::remove_cvref_t<decltype(typeWrapper)>::type;
                
                if (auto it = cache->find(name); it != cache->end())
                {
                    std::shared_ptr<concept_t> shrdPtr = it->second.lock();

                    if (!shrdPtr)
                    {
                        std::shared_ptr<model<T>> result = producer();
                        it->second = result;
                        return result;
                    }
                    return cast<T>(shrdPtr);
                }
                else {
                    throw std::runtime_error("Failed to find resource in the cache");
                }
            }
            template<typename TupleRef>
            auto operator()([[maybe_unused]] safety, std::string_view name, TupleRef const& tupleRef) const noexcept
                -> get_variant<TupleRef>
            {
                auto const& producer = hana::at_c<0>(tupleRef);
                auto& cache = hana::at_c<1>(tupleRef);
                auto typeWrapper = hana::at_c<2>(tupleRef);

                using T = typename std::remove_cvref_t<decltype(typeWrapper)>::type;

                if (auto it = cache->find(name); it != cache->end())
                {
                    std::shared_ptr<concept_t> shrdPtr = it->second.lock();

                    if (!shrdPtr)
                    {
                        std::shared_ptr<model<T>> result = producer();
                        it->second = result;
                        return result;
                    }
                    return cast<T>(shrdPtr);
                }
                return hana::make_tuple(producer(), cache, name, typeWrapper);
            }
            template<typename TupleRef>
            constexpr auto operator()([[maybe_unused]] full, std::string_view name, TupleRef const& tupleRef) const noexcept
                -> get_tuple<TupleRef>
            {
                auto const& producer = hana::at_c<0>(tupleRef);
                auto& cache = hana::at_c<1>(tupleRef);
                auto typeWrapper = hana::at_c<2>(tupleRef);

                return hana::make_tuple(producer(), cache, name, typeWrapper);
            }
        };

        struct get_t {
            [[nodiscard]] constexpr auto operator()(safety safe, std::string_view name) const noexcept {
                return hana::partial(continuation_get_t{}, safe, name);
            }
            [[nodiscard]] constexpr auto operator()(perf performance, std::string_view name) const {
                return hana::partial(continuation_get_t{}, performance, name);
            }
            [[nodiscard]] constexpr auto operator()(full full_pipeline, std::string_view name) const noexcept {
                return hana::partial(continuation_get_t{}, full_pipeline, name);
            }
            [[nodiscard]] constexpr auto operator()(std::string_view name) const noexcept {
                return this->operator()(safety{}, name);
            }  
        };
    }

    inline constexpr detail::get_t get{};
}

#endif /* GET_HPP */
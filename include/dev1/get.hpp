// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.

#ifndef GET_HPP
#define GET_HPP

#include <string>
#include <stdexcept>
#include "detail/cache_map.hpp"
#include "detail/get_proxy.hpp"
#include "detail/produce_proxy.hpp"

namespace dev1::detail {

    template<typename ProducerFn, typename T>
    struct get_miss_t {
        [[no_unique_address]] ProducerFn producer;
        std::string_view name;
    };

    template<typename ProducerFn, typename T>
    struct get_stage {
        using resource_type = T;
        using producer_type = ProducerFn;
        using miss_type = get_miss_t<ProducerFn, T>;

        [[no_unique_address]] ProducerFn producer;
        std::string name;

        template<typename Continuation>
        requires std::invocable<Continuation, miss_type const&> &&
         std::same_as<
             std::invoke_result_t<Continuation, miss_type const&>,
             std::shared_ptr<model<T>>
         >
        [[nodiscard]] std::shared_ptr<model<T>> chain(cache_map& cache, Continuation&& cont) const {
            if (auto it = cache.find(name); it != cache.end()) {
                if (std::shared_ptr<concept_t> base = it->second.lock()) {
                    return { base, static_cast<model<T>*>(base.get()) };
                }
                auto resource_ptr = std::forward<Continuation>(cont)(
                    miss_type{ producer, name }
                );
                it->second = resource_ptr; 
                return resource_ptr;
            }

            return std::forward<Continuation>(cont)(
                miss_type{ producer, name }
            );
        }
        

        [[nodiscard]] std::shared_ptr<model<T>> operator()(cache_map& cache) const {
            return chain(cache, [&name](miss_type const&) -> std::shared_ptr<model<T>> {
                throw std::runtime_error("Cache miss: " + std::string{name});
            });
        }
    };

    struct get_t {
        [[nodiscard]] auto operator()(std::string_view name) const noexcept {
            return get_proxy{ name };
        }
    };

    template<typename ProducerFn, typename T>
    [[nodiscard]] auto operator|(produce_proxy<ProducerFn, T> pp, get_proxy gp) {
        return get_stage<ProducerFn, T>{
            std::move(pp.producer),
            gp.name
        };
    }

    template<typename ProducerFn, typename T>
    [[nodiscard]] auto operator|(cache_map* cache, get_stage<ProducerFn, T> const& stage) {
        return stage(cache);
    }


} 

namespace dev1 {

    inline constexpr detail::get_t get{};
    
}

#endif
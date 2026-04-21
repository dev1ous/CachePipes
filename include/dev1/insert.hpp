// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.

#ifndef INSERT_HPP
#define INSERT_HPP

#include "cache_map.hpp"
#include "detail/concept.hpp"

namespace dev1::detail {

    template<typename PrevStage>
    struct insert_stage {
        using T = typename PrevStage::resource_type;

        PrevStage prev;

        [[nodiscard]] std::shared_ptr<model<T>> operator()(cache_map* cache) const {
            return prev.chain(cache, [&cache](typename PrevStage::miss_type const& miss) {
                cache->emplace(
                    miss.name,
                    miss.resource_ptr
                );
                return miss.resource_ptr;
            });
        }
    };

    struct insert_proxy{};

    struct insert_t {
        [[nodiscard]] auto operator()() const noexcept { 
            return insert_proxy{};
        }
    };

    template<impl::PipelineStage PrevStage>
    [[nodiscard]] auto operator|(PrevStage&& prev, insert_proxy) noexcept {
        return insert_stage<std::remove_cvref_t<PrevStage>>{
            std::forward<PrevStage>(prev)
        };
    }

    template<typename PrevStage>
    [[nodiscard]] auto operator|(cache_map* cache, insert_stage<PrevStage> const& stage) {
        return stage(cache);
    }
}

namespace dev1 {

    inline constexpr detail::insert_t insert{};

}

#endif
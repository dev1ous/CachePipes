// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.

#ifndef LOAD_RESOURCE_HPP
#define LOAD_RESOURCE_HPP

#include <stdexcept>
#include "solve_filepath.hpp"
#include "detail/concept.hpp"

namespace dev1::detail {

    struct default_loader_t {};

    template<typename T>
    struct load_miss_t {
        std::shared_ptr<model<T>> resource_ptr;
        std::string_view name;
    };

    template<typename PrevStage, typename LoaderFn = default_loader_t>
    struct load_stage {
        using T = typename PrevStage::resource_type;
        using ProducerFn = typename PrevStage::producer_type;
        using resource_type = T;
        using producer_type = ProducerFn;
        using miss_type = load_miss_t<T>;

        PrevStage prev;
        [[no_unique_address]] LoaderFn loader;

        template<typename Continuation>
        [[nodiscard]] std::shared_ptr<model<T>> chain(cache_map& cache, Continuation&& cont) const {
            return prev.chain(cache, [&loader, &cont](typename PrevStage::miss_type const& miss) {
                auto resource_ptr = miss.producer();
                T& resource = static_cast<T&>(*resource_ptr);

                bool loaded;
                if constexpr (std::is_same_v<LoaderFn, default_loader_t>) {
                    static_assert(validator<T>::is_valid_loader,
                        "traits<T>::loader() must accept (T&, path const&) or "
                        "(T&, string const&) and return bool.");
                    loaded = validator<T>::loader(resource, miss.filepath);
                } else {
                    static_assert(impl::ValidResourceHandler<LoaderFn, T>,
                        "Custom loader must be callable as loader(T&, path const&).");
                    loaded = loader(resource, miss.filepath);
                }
                if (!loaded)
                    throw std::runtime_error("Failed to load resource: " + miss.name);

                return std::forward<Continuation>(cont)(
                    miss_type{ resource_ptr, miss.name }
                );
            });
        }

        [[nodiscard]] std::shared_ptr<model<T>> operator()(cache_map& cache) const {
            return chain(cache, [&loader](miss_type const&) -> std::shared_ptr<model<T>> {
                throw std::runtime_error("Cache miss: " + std::string{name});
            });
        }
    };

    template<typename Fn = default_loader_t>
    struct load_resource_proxy {
        [[no_unique_address]] Fn func;
    };

    struct load_resource_t {
        [[nodiscard]] auto operator()() const noexcept {
            return load_resource_proxy<default_loader_t>{};
        }
        template<typename Fn>
        [[nodiscard]] auto operator()(Fn&& fn) const noexcept {
            return load_resource_proxy<std::remove_cvref_t<Fn>>{ std::forward<Fn>(fn) };
        }
    };

    template<impl::PipelineStage PrevStage>
    [[nodiscard]] auto operator|(PrevStage&& prev, load_resource_proxy<default_loader_t>) noexcept {
        return load_stage<std::remove_cvref_t<PrevStage>>{
            std::forward<PrevStage>(prev)
        };
    }

    template<impl::PipelineStage PrevStage, typename Fn>
    [[nodiscard]] auto operator|(PrevStage&& prev, load_resource_proxy<Fn> proxy) noexcept {
        return load_stage<std::remove_cvref_t<PrevStage>, Fn>{
            std::forward<PrevStage>(prev), std::move(proxy.func)
        };
    }

}

namespace dev1 {

    inline constexpr detail::load_resource_t load_resource{};
    
}

#endif 
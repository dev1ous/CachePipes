// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.

#ifndef SOLVE_FILEPATH_HPP
#define SOLVE_FILEPATH_HPP

#include <filesystem>
#include <stdexcept>
#include "get.hpp"
#include "traits.hpp"

namespace dev1::detail {

    template<typename ProducerFn, typename T>
    struct solve_miss_t {
        [[no_unique_address]] ProducerFn producer;
        std::string_view name;
        std::filesystem::path const& filepath;
    };

    template<typename T>
    [[nodiscard]] std::filesystem::path find_filepath(
        std::string_view base_path, std::string_view name)
    {
        static_assert(validator<T>::template is_valid_extensions<T>,
            "traits<T>::extensions() must return a fixed-size std::array<std::string_view, N>."
        );

        if constexpr (validator<T>::template is_valid_extensions<T>) {
            constexpr auto exts = validator<T>::extensions();
            std::filesystem::path root = std::filesystem::path(EXECUTABLE_DIR) / base_path;
            for (auto const& entry : std::filesystem::recursive_directory_iterator(root, std::filesystem::directory_options::skip_permission_denied)) {
                auto const& path = entry.path();
                if (path.stem().string() != name) {
                    continue;
                }
                for (auto const& ext : exts) {
                    if (path.extension().string() == ext) {
                        return path;
                    }
                }
            }
        }
        return {};
    }

    template<typename PrevStage>
    struct solve_stage {
        using T = typename PrevStage::resource_type;
        using ProducerFn = typename PrevStage::producer_type;
        using resource_type = T;
        using producer_type = ProducerFn;
        using miss_type = solve_miss_t<ProducerFn, T>;

        PrevStage prev;
        std::string base_path;

        template<typename Continuation>
        [[nodiscard]] std::shared_ptr<model<T>> chain(cache_map& cache, Continuation&& cont) const {
            return prev.chain(cache, [&base_path, &cont](typename PrevStage::miss_type const& miss) {
                auto filepath = find_filepath<T>(base_path, miss.name);
                if (filepath.empty()) {
                    throw std::runtime_error("Failed to find resource: " + miss.name);
                }
                return std::forward<Continuation>(cont)(
                    miss_type{ miss.producer, miss.name, std::move(filepath) }
                );
            });
        }
    };

    struct solve_filepath_proxy { std::string_view base_path; };

    struct solve_filepath_t {
        [[nodiscard]] auto operator()(std::string_view base_path) const noexcept {
            return solve_filepath_proxy{ base_path };
        }
    };

    template<impl::PipelineStage PrevStage>
    [[nodiscard]] auto operator|(PrevStage&& prev, solve_filepath_proxy proxy) noexcept {
        return solve_stage<std::remove_cvref_t<PrevStage>>{
            std::forward<PrevStage>(prev), proxy.base_path
        };
    }

} 

namespace dev1 {

    inline constexpr detail::solve_filepath_t solve_filepath{};

}

#endif
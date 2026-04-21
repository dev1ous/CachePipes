// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#ifndef ALGOS_HPP
#define ALGOS_HPP

#include "detail/produce.hpp"
#include "detail/get.hpp"
#include "detail/load_resource.hpp"
#include "detail/solve_filepath.hpp"
#include "detail/insert.hpp"

namespace dev1::detail {
    struct pipeline_tag {};

    class unordered_map;
    template<typename T> struct pipeline_safety;
    template<typename T> struct pipeline_perf;
    template<typename T> struct pipeline_full;
    template<typename T> struct pipeline_type;

    template<typename Lazy>
    auto operator|(pipeline_safety<Lazy>&& pipe1, solve_filepath_t::proxy const& pipe2);

    template<typename Lazy, typename Func>
    auto operator|(pipeline_safety<Lazy>&& pipe1, load_resource_t::proxy<Func> const& pipe2);

    template<typename Lazy>
    auto operator|(pipeline_safety<Lazy>&& pipe1, insert_t::proxy const& pipe2);

    template<typename Lazy, typename Partial>
    auto operator|(pipeline_perf<Lazy>&& pipe1, [[maybe_unused]] Partial&& pipe2);

    template<typename Lazy>
    auto operator|(pipeline_full<Lazy>&& pipe1, solve_filepath_t::proxy const& pipe2);

    template<typename Lazy, typename Func>
    auto operator|(pipeline_full<Lazy>&& pipe1, load_resource_t::proxy<Func> const& pipe2);

    template<typename Lazy>
    auto operator|(pipeline_full<Lazy>&& pipe1, insert_t::proxy const& pipe2);

    template <typename Lazy, typename Partial>
    auto operator|(pipeline_type<Lazy>&& pipe1, Partial&& pipe2) 
        requires impl::FirstParamOfType<perf, Partial>;

    template <typename Lazy, typename Partial>
    auto operator|(pipeline_type<Lazy>&& pipe1, Partial&& pipe2)
         requires impl::FirstParamOfType<safety, Partial>;

    template <typename Lazy, typename Partial>
    auto operator|(pipeline_type<Lazy>&& pipe1, Partial&& pipe2)
        requires impl::FirstParamOfType<full, Partial>;

    template <typename T>
    struct pipeline_safety {
        using hana_tag = pipeline_tag;
        
        constexpr explicit pipeline_safety(T&& val) : value(std::forward<T>(val)) {}

        template<typename Lazy>
        friend auto operator|(pipeline_safety<Lazy>&&, solve_filepath_t::proxy const&);
        
        template<typename Lazy, typename Func>
        friend auto operator|(pipeline_safety<Lazy>&&, load_resource_t::proxy<Func> const&);
        
        template<typename Lazy>
        friend auto operator|(pipeline_safety<Lazy>&&, insert_t::proxy const&);

        [[nodiscard]] constexpr decltype(auto) operator()() const
        {
            using ReturnType = std::remove_cvref_t<decltype((hana::eval(value)))>;
    
            static_assert(impl::IsSharedPtr<ReturnType>, 
                        "You use a tag full or perf in a non-finished pipeline");

            return hana::eval(value);
        }
    private:
        T value;
    };

    template <typename T>
    struct pipeline_perf {
        using hana_tag = pipeline_tag;
        
        constexpr explicit pipeline_perf(T&& val) : value(std::forward<T>(val)) {}

        template<typename Lazy, typename Partial>
        friend auto operator|(pipeline_perf<Lazy>&&, [[maybe_unused]] Partial&&);

        [[nodiscard]] decltype(auto) operator()() const
        {
            return hana::eval(value);
        }
    private:
        T value;
    };

    template <typename T>
    struct pipeline_full {
        using hana_tag = pipeline_tag;
        
        constexpr explicit pipeline_full(T&& val) : value(std::forward<T>(val)) {}

        template<typename Lazy>
        friend auto operator|(pipeline_full<Lazy>&&, solve_filepath_t::proxy const&);
        
        template<typename Lazy, typename Func>
        friend auto operator|(pipeline_full<Lazy>&&, load_resource_t::proxy<Func> const&);
        
        template<typename Lazy>
        friend auto operator|(pipeline_full<Lazy>&&, insert_t::proxy const&);

        [[nodiscard]] decltype(auto) operator()() const
        {
            using ReturnType = std::remove_cvref_t<decltype((hana::eval(value)))>;
    
            static_assert(impl::IsSharedPtr<ReturnType>, 
                        "You use a tag full or perf in a non-finished pipeline");

            return hana::eval(value);
        }
    private:
        T value;
    };

    template <typename T>
    struct pipeline_type {
        using hana_tag = pipeline_tag;
        
        constexpr explicit pipeline_type(T&& val) : value(std::forward<T>(val)) {}

        template <typename Lazy, typename Partial>
        friend auto operator|(pipeline_type<Lazy>&& pipe1, Partial&& pipe2)
            requires impl::FirstParamOfType<perf, Partial>;

        template <typename Lazy, typename Partial>
        friend auto operator|(pipeline_type<Lazy>&& pipe1, Partial&& pipe2)
            requires impl::FirstParamOfType<safety, Partial>;

        template <typename Lazy, typename Partial>
        friend auto operator|(pipeline_type<Lazy>&& pipe1, Partial&& pipe2)
            requires impl::FirstParamOfType<full, Partial>;

    private:
        T value;
    };

    template<typename Lazy>
    auto operator|(pipeline_safety<Lazy>&& pipe1, solve_filepath_t::proxy const& pipe2)
    {
        using TransformResult = decltype(
            hana::transform(
                std::declval<Lazy>(),
                pipe2.template dispatch<safety>()
            )
        );
        return pipeline_safety<TransformResult>(
            hana::transform(
                std::move(pipe1.value),
                pipe2.template dispatch<safety>()
            )
        );
    }

    template<typename Lazy, typename Func>
    auto operator|(pipeline_safety<Lazy>&& pipe1, load_resource_t::proxy<Func> const& pipe2)
    {
        using TransformResult = decltype(
            hana::transform(
                std::declval<Lazy>(),
                pipe2.template dispatch<safety>()
            )
        );
        return pipeline_safety<TransformResult>(
            hana::transform(
                std::move(pipe1.value),
                pipe2.template dispatch<safety>()
            )
        );
    }

    template<typename Lazy>
    auto operator|(pipeline_safety<Lazy>&& pipe1, insert_t::proxy const& pipe2)
    {
        using TransformResult = decltype(
            hana::transform(
                std::declval<Lazy>(),
                pipe2.template dispatch<safety>()
            )
        );
        return pipeline_safety<TransformResult>(
            hana::transform(
                std::move(pipe1.value),
                pipe2.template dispatch<safety>()
            )
        );
    }

    template<typename Lazy, typename Partial>
    auto operator|(pipeline_perf<Lazy>&& pipe1, [[maybe_unused]] Partial&& pipe2)
    {
        return pipeline_perf(std::move(pipe1.value));
    }

    template<typename Lazy>
    auto operator|(pipeline_full<Lazy>&& pipe1, solve_filepath_t::proxy const& pipe2)
    {
        using TransformResult = decltype(
            hana::transform(
                std::declval<Lazy>(),
                pipe2.template dispatch<full>()
            )
        );
        return pipeline_full<TransformResult>(
            hana::transform(
                std::move(pipe1.value),
                pipe2.template dispatch<full>()
            )
        );
    }
        
    template<typename Lazy, typename Func>
    auto operator|(pipeline_full<Lazy>&& pipe1, load_resource_t::proxy<Func> const& pipe2)
    {
        using TransformResult = decltype(
            hana::transform(
                std::declval<Lazy>(),
                pipe2.template dispatch<full>()
            )
        );
        return pipeline_full<TransformResult>(
            hana::transform(
                std::move(pipe1.value),
                pipe2.template dispatch<full>()
            )
        );
    }
        
    template<typename Lazy>
    auto operator|(pipeline_full<Lazy>&& pipe1, insert_t::proxy const& pipe2)
    {
        using TransformResult = decltype(
            hana::transform(
                std::declval<Lazy>(),
                pipe2.template dispatch<full>()
            )
        );
        return pipeline_full<TransformResult>(
            hana::transform(
                std::move(pipe1.value),
                pipe2.template dispatch<full>()
            )
        );
    }

    template <typename Lazy, typename Partial>
    auto operator|(pipeline_type<Lazy>&& pipe1, Partial&& pipe2)
        requires impl::FirstParamOfType<perf, Partial>
    {
        return pipeline_perf(
            hana::transform(
                std::move(pipe1.value),
                std::move(pipe2)
            )
        );
    }

    template <typename Lazy, typename Partial>
    auto operator|(pipeline_type<Lazy>&& pipe1, Partial&& pipe2)
        requires impl::FirstParamOfType<safety, Partial>
    {
        return pipeline_safety(
            hana::transform(
                std::move(pipe1.value),
                std::move(pipe2)
            )
        );
    }

    template <typename Lazy, typename Partial>
    auto operator|(pipeline_type<Lazy>&& pipe1, Partial&& pipe2)
        requires impl::FirstParamOfType<full, Partial>
            
    {
        return pipeline_full(
            hana::transform(
                std::move(pipe1.value),
                std::move(pipe2)
            )
        );
    }
}

namespace boost::hana {
    template <typename T>
    struct tag_of<dev1::detail::pipeline_safety<T>> {
        using type = dev1::detail::pipeline_tag;
    };
    template <typename T>
    struct tag_of<dev1::detail::pipeline_perf<T>> {
        using type = dev1::detail::pipeline_tag;
    };
    template <typename T>
    struct tag_of<dev1::detail::pipeline_full<T>> {
        using type = dev1::detail::pipeline_tag;
    };
    template <typename T>
    struct tag_of<dev1::detail::pipeline_type<T>> {
        using type = dev1::detail::pipeline_tag;
    };

    template<typename Func>
    auto operator|(dev1::detail::unordered_map* cache, Func&& func)
        requires requires(Func&& f, dev1::detail::unordered_map* c) {
            { f(c) } -> dev1::detail::impl::LazyApply;
        }
    {
        return dev1::detail::pipeline_type(std::forward<Func>(func)(cache));
    }
}

#endif
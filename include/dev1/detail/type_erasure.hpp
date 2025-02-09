// Copyright Gregoire Faltrauer 2025
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 
#ifndef TYPE_ERASURE_HPP
#define TYPE_ERASURE_HPP

#include <utility>

namespace dev1 {
    namespace detail {
        struct concept_t {
            virtual ~concept_t() = default;
        };

        template <typename T>
        struct model final : concept_t {
            model() = default;
    
            explicit model(T&& data) : _data(std::forward<T>(data)) {}
            
            explicit model(T const& data) : _data(data) {}

            template<typename... Args>
            explicit model(Args&&... args) : _data(std::forward<Args>(args)...) {}

            model(model const& other) = default;
            model& operator=(model const& other) = default;
            
            model(model&& other) = default;
            model& operator=(model&& other) = default;
            
            ~model() = default;

            [[nodiscard]] operator T&() & noexcept { return _data; }
            [[nodiscard]] operator const T&() const& noexcept{ return _data; }

        private:
            T _data;
        };
    }
}
#endif
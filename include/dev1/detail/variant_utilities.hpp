#ifndef VARIANT_UTILITIES_HPP
#define VARIANT_UTILITIES_HPP

#include <variant>
#include <utility>

namespace dev1::detail {
    template<size_t I, typename...Ts>
    auto const& get_unbound(std::variant<Ts...> const& v) noexcept {
        if (auto* pv = std::get_if<I>(&v)) {
            return *pv;
        } else {
            std::unreachable();
        }
    }
}

#endif
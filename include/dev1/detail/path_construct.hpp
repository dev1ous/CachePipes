#ifndef PATH_CONSTRUCT_HPP
#define PATH_CONSTRUCT_HPP

#include <string_view>

#include <boost/hana/map.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/basic_tuple.hpp>

namespace dev1 {
    namespace detail {
        namespace hana = boost::hana;

        constexpr auto get_extension(auto str) {
            constexpr auto dot_pos = hana::reverse(hana::find(str, hana::char_c<'.'>));
            return dot_pos ? hana::drop(str, hana::size_c<*dot_pos + 1>) : hana::string<>{};
        }
    }

    template <typename CharT, CharT... Cs>
    constexpr auto operator""_r() {
        return hana::string<Cs...>;
    }
}

#endif
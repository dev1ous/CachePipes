#ifndef CACHE_MAP_HPP
#define CACHE_MAP_HPP

#include "map.hpp"
#include "type_erasure.hpp"

namespace dev1::detail {
    using cache_map = unordered_map<std::string, std::weak_ptr<concept_t>>;
}

#endif
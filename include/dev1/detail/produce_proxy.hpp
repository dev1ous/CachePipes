#ifndef PRODUCE_PROXY_HPP
#define PRODUCE_PROXY_HPP

namespace dev1::detail {
    template<typename Func, typename T>
    struct produce_proxy {
        using resource_type = T;
        
        [[no_unique_address]] Func func;
    };
}

#endif
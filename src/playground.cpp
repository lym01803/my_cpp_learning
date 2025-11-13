#include <algorithm>
#include <concepts>
#include <iostream>
#include <playground.h>
#include <ranges>
#include <optional>
#include <tuple>
#include <utility>

namespace playground {

void get_right_triangle_triples() {
    const auto for_each = []<typename F>(F&& f) {
        return std::views::transform(std::forward<F>(f)) | std::views::join;
    };
    long long counter {0LL};
    auto seq = std::views::iota(1LL) 
        | for_each([&for_each, &counter](const long long c){
            return std::views::iota(1LL, c) 
                | for_each([=, &for_each, &counter](const long long a) {
                    return std::views::iota(a, c) 
                        | for_each([=, &for_each, &counter](const long long b) {
                            counter += 1;
                            return a * a + b * b == c * c
                                ? optional_view {std::tuple {a, b, c}}
                                : optional_view<decltype(std::tuple {a, b, c})> {};
                        });
                });
        });
    long long s {0LL};
    for (const auto & item : seq | std::views::take(1000)) {
        // std::cout << std::get<0>(item) << " "
        //     << std::get<1>(item) << " "
        //     << std::get<2>(item) << std::endl;
        s += std::get<2>(item);
    }
    std::cout << std::format("counter = {}, s = {}", counter, s) << std::endl;
}

} // namespace playground

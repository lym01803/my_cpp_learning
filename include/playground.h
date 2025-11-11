#pragma once

#include <algorithm>
#include <concepts>
#include <vector>
#include <optional>
#include <ranges>
#include <iostream>

namespace playground {

template <std::semiregular T>
struct optional_view: public std::ranges::view_interface<optional_view<T> > {
    optional_view() = default;
    optional_view(T data): data(std::move(data)) {}

    T const * begin() const noexcept {
        return data ? &data.value() : nullptr;
    }
    T const * end() const noexcept {
        return data ? &data.value() + 1 : nullptr;
    }
private:
    std::optional<T> data{};
};

inline auto get_right_triangle_triples(int n) {
    const auto for_each = []<typename F>(F&& f) {
        return std::views::transform(std::forward<F>(f)) | std::views::join;
    };
    auto seq = std::views::iota(1LL) 
        | for_each([&for_each](const long long c){
            return std::views::iota(1LL, c) 
                | for_each([=, &for_each](const long long a) {
                    return std::views::iota(a, c) 
                        | for_each([=, &for_each](const long long b) {
                            return a * a + b * b == c * c
                                ? optional_view {std::tuple {a, b, c}}
                                : optional_view<decltype(std::tuple {a, b, c})> {};
                        });
                });
        });
    return seq | std::views::take(n);
}

inline void ranges_sort(int n) {
    std::vector vec {std::from_range, get_right_triangle_triples(n)};
    std::ranges::sort(vec, std::ranges::less {}, [](const auto &t) { return std::get<0>(t); });
    for (const auto &t : vec) {
        std::cout << std::get<0>(t) << " " << std::get<1>(t) << " "
            << std::get<2>(t) << std::endl;
    }
    std::cout << std::endl;
}

} // namespace playground

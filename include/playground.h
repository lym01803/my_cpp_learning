#pragma once

#include <concepts>
#include <functional>
#include <optional>
#include <ranges>
#include <type_traits>

namespace playground {

void get_right_triangle_triples();

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

} // namespace playground

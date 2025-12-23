#pragma once

#include <tuple>
#include <utility>

namespace my {

template <typename... Ts>
struct tuple;

template <>
struct tuple<> {};

template <typename T, typename... Ts>
struct tuple<T, Ts...> {
  T value;
  tuple<Ts...> others;

  tuple(T v, Ts... vs) : value{std::forward<T>(v)}, others{std::forward<Ts>(vs)...} {}
};

template <size_t I, typename... Ts>
auto get(const tuple<Ts...>& tp) -> const typename std::tuple_element_t<I, std::tuple<Ts...>>& {
  if constexpr (I == 0) {
    return tp.value;
  } else {
    return get<I - 1>(tp.others);
  }
}

template <size_t I, typename... Ts>
auto get(tuple<Ts...>& tp) -> typename std::tuple_element_t<I, std::tuple<Ts...>>& {
  if constexpr (I == 0) {
    return tp.value;
  } else {
    return get<I - 1>(tp.others);
  }
}

}  // namespace my

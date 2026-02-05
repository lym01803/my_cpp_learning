#pragma once

#include <algorithm>
#include <any>
#include <atomic>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <coroutine>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <queue>
#include <ranges>
#include <semaphore>
#include <stdexcept>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "toyqueue.h"
#include "async_tool.h"

namespace playground {

template <std::semiregular T>
struct optional_view : public std::ranges::view_interface<optional_view<T>> {
  optional_view() = default;
  optional_view(T data) : data(std::move(data)) {}

  T const* begin() const noexcept {
    return data ? &data.value() : nullptr;
  }
  T const* end() const noexcept {
    return data ? &data.value() + 1 : nullptr;
  }

 private:
  std::optional<T> data{};
};

inline auto get_right_triangle_triples(int n) {
  const auto for_each = []<typename F>(F&& f) {
    return std::views::transform(std::forward<F>(f)) | std::views::join;
  };
  auto seq = std::views::iota(1LL) | for_each([&for_each](const long long c) {
               return std::views::iota(1LL, c) | for_each([=, &for_each](const long long a) {
                        return std::views::iota(a, c) | for_each([=, &for_each](const long long b) {
                                 return a * a + b * b == c * c
                                            ? optional_view{std::tuple{a, b, c}}
                                            : optional_view<decltype(std::tuple{a, b, c})>{};
                               });
                      });
             });
  return seq | std::views::take(n);
}

inline void ranges_sort(int n) {
  std::vector vec{std::from_range, get_right_triangle_triples(n)};
  std::ranges::sort(vec, std::ranges::less{}, [](const auto& t) { return std::get<0>(t); });
  for (const auto& t : vec) {
    std::cout << std::get<0>(t) << " " << std::get<1>(t) << " " << std::get<2>(t) << std::endl;
  }
  std::cout << std::endl;
}

void try_concurrency();

void try_mutex();

template <std::invocable F>
  requires std::move_constructible<F>
auto timer_wrap(F f) {
  return [f = std::move(f)]() mutable {
    const auto start_time = std::chrono::high_resolution_clock::now();
    std::invoke(f);
    return std::chrono::high_resolution_clock::now() - start_time;
  };
}

template <typename T>
struct Matrix2D {
  Matrix2D(size_t row, size_t col) : row{row}, col{col} {
    alloc();
  }

  Matrix2D(Matrix2D&& other) noexcept = default;
  Matrix2D& operator=(Matrix2D&& other) noexcept = default;

  Matrix2D(const Matrix2D& other) : Matrix2D{other.row, other.col} {
    copy_from(other);
  }

  template <typename U>
  Matrix2D(const Matrix2D<U>& other) : Matrix2D{other.row, other.col} {
    copy_from(other);
  }

  Matrix2D& operator=(const Matrix2D& other) {
    if (this == &other) {
      return *this;
    }
    *this = Matrix2D{other};
    return *this;
  }

  template <typename U>
  Matrix2D& operator=(const Matrix2D<U>& other) {
    if (this == &other) {
      return *this;
    }
    *this = Matrix2D{other};
    return *this;
  }

  T& operator[](size_t r, size_t c) {
    if (data) {
      if (r < row && c < col) {
        return data[r][c];
      }
      throw std::runtime_error("index (r, c) is out of range.");
    }
    throw std::runtime_error("data of Matrix2D is nullptr.");
  }

  const T& operator[](size_t r, size_t c) const {
    if (data) {
      if (r < row && c < col) {
        return data[r][c];
      }
      throw std::runtime_error("index (r, c) is out of range.");
    }
    throw std::runtime_error("data of Matrix2D is nullptr.");
  }

  friend std::ostream& operator<<(std::ostream& os, const Matrix2D& obj) {
    if (obj.data) {
      for (size_t r = 0; r < obj.row; r++) {
        for (size_t c = 0; c < obj.col; c++) {
          os << obj[r, c] << ' ';
        }
        os << std::endl;
      }
    }
    return os;
  }

 private:
  std::unique_ptr<std::unique_ptr<T[]>[]> data{};
  size_t row{};
  size_t col{};

  template <typename U>
  friend class Matrix2D;

  void alloc() {
    std::unique_ptr<std::unique_ptr<T[]>[]> _data{new std::unique_ptr<T[]>[row] {}};
    for (size_t r = 0; r < row; r++) {
      _data[r] = std::unique_ptr<T[]>{new T[col]{}};
    }
    data = std::move(_data);
  }

  template <typename U>
    requires std::convertible_to<U, T> && std::copy_constructible<T>
  void copy_from(const Matrix2D<U>& other) {
    for (size_t r = 0; r < row; r++) {
      std::copy(other.data[r].get(), other.data[r].get() + col, data[r].get());
    }
  }
};

void play_with_matrix();

void try_shared_mutex();

void try_condition_variable();

void try_condition_variable_with_stop();

}  // namespace playground

namespace msg {

enum class toy_msg : uint8_t { _int, _double, _string, _vec_int };

}  // namespace msg

namespace playground {

void try_message();

void try_msg_stream();

void try_coroutine();

void try_toy_queue();

void try_toy_queue2();

namespace toy_func_type {

template <std::movable F>
class task {
  F f;

 public:
  task(F f) : f(std::move(f)) {}

  template <typename... Args>
    requires std::invocable<F, Args...>
  auto run(Args&&... args) {
    return std::invoke(f, std::forward<Args>(args)...);
  }
};

template <typename... Args>
void f(Args&&... args) {
  ((std::cout << args << " "), ...);
  std::cout << std::endl;
}

inline void toy_task_test() {
  auto task_to_run = [&]<typename... Args>(Args&&... args) {
    task t{f<Args...>};
    return t.run(std::forward<Args>(args)...);
  };
  task_to_run(1, 2);
}

}  // namespace toy_func_type
void try_toy_queue3();

void try_toy_duck_type();

void try_await();

void try_await2();

void try_await3();

void try_await4();

void try_await5();

void try_await6();

void try_await7();

void try_await8();

void try_await9();

void try_await10();

void try_await11();

void try_await12();

void try_await13();

}  // namespace playground

#pragma once

#include <algorithm>
#include <chrono>
#include <concepts>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

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
               return std::views::iota(1LL, c) |
                      for_each([=, &for_each](const long long a) {
                        return std::views::iota(a, c) |
                               for_each([=, &for_each](const long long b) {
                                 return a * a + b * b == c * c
                                            ? optional_view{std::tuple{a, b, c}}
                                            : optional_view<decltype(std::tuple{
                                                  a, b, c})>{};
                               });
                      });
             });
  return seq | std::views::take(n);
}

inline void ranges_sort(int n) {
  std::vector vec{std::from_range, get_right_triangle_triples(n)};
  std::ranges::sort(vec, std::ranges::less{},
                    [](const auto& t) { return std::get<0>(t); });
  for (const auto& t : vec) {
    std::cout << std::get<0>(t) << " " << std::get<1>(t) << " "
              << std::get<2>(t) << std::endl;
  }
  std::cout << std::endl;
}

void try_concurrency();

void try_mutex();

class guarded_thread {
  std::thread t;

 public:
  guarded_thread(std::thread t) : t(std::move(t)) {}
  guarded_thread(const guarded_thread& other) = delete;
  guarded_thread(guarded_thread&& other) noexcept = default;
  guarded_thread& operator=(const guarded_thread& other) = delete;
  guarded_thread& operator=(guarded_thread&& other) noexcept {
    if (t.joinable()) {
      try {
        t.join();
      } catch (...) {
        std::terminate();
      }
    }
    t = std::move(other.t);
    return *this;
  }

  ~guarded_thread() {
    if (t.joinable()) {
      t.join();
    }
  }

  std::thread& operator*() noexcept {
    return t;
  }
  std::thread const& operator*() const noexcept {
    return t;
  }
  std::thread* operator->() noexcept {
    return &t;
  }
  std::thread const* operator->() const noexcept {
    return &t;
  }
  std::thread release() noexcept {
    return std::move(t);
  }
};

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
    std::unique_ptr<std::unique_ptr<T[]>[]> _data{
        new std::unique_ptr<T[]>[row] {}};
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

void play_with_tick_timer();

struct tick_timer {
  using duration_t = std::chrono::nanoseconds;
  using time_point_t = std::chrono::high_resolution_clock::time_point;

  tick_timer() {}
  tick_timer(const tick_timer& other) : interval{other.interval} {}
  tick_timer(tick_timer&& other) noexcept = default;
  tick_timer& operator=(const tick_timer& other) noexcept {
    if (this == &other) {
      return *this;
    }
    interval = other.interval;
    return *this;
  }
  tick_timer& operator=(tick_timer&& other) noexcept = default;
  ~tick_timer() {
    running = false;
  }

  template <typename R, typename P>
  tick_timer(const std::chrono::duration<R, P>& interval)
      : interval{interval} {}

  template <typename R, typename P>
    requires requires(duration_t t, const std::chrono::duration<R, P>& other) {
      t = other;
    }
  tick_timer& operator=(const std::chrono::duration<R, P>& interval) noexcept {
    this->interval = interval;
    return *this;
  }

  time_point_t operator()() const noexcept {
    return time_point;
  }

 private:
  duration_t interval{std::chrono::milliseconds(1)};
  time_point_t time_point{std::chrono::high_resolution_clock::now()};
  bool running{true};
  guarded_thread th{std::thread{[this]() { this->tick(); }}};

  void tick() {
    while (running) {
      time_point = std::chrono::high_resolution_clock::now();
      std::this_thread::sleep_for(interval);
    }
  }
};

}  // namespace playground

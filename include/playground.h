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
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "message.h"

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

struct stopable_cv {
  stopable_cv() noexcept = default;
  ~stopable_cv() {
    stop();  // UB if there is thread waiting for this cv, but nothing else can I do.
  }

  bool is_stoped() const noexcept {
    return stoped.load();
  }

  void stop() noexcept {
    stoped = true;
    cv.notify_all();
  }

  std::condition_variable& operator*() noexcept {
    return cv;
  }
  std::condition_variable const& operator*() const noexcept {
    return cv;
  }
  std::condition_variable* operator->() noexcept {
    return &cv;
  }
  std::condition_variable const* operator->() const noexcept {
    return &cv;
  }

 private:
  std::condition_variable cv;
  std::atomic<bool> stoped{false};
};

}  // namespace playground

namespace msg {

enum class toy_msg : uint8_t { _int, _double, _string, _vec_int };

}  // namespace msg

namespace playground {

template <typename T = void>
struct coroutine_object;

template <>
struct coroutine_object<void> {
  struct promise_type {
    bool done{false};
    auto get_return_object() {
      return coroutine_object{std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    auto initial_suspend() {
      return std::suspend_never{};
    }
    auto final_suspend() noexcept {
      return std::suspend_always{};
    }
    void return_void() {
      done = true;
    }
    void unhandled_exception() {}
  };

  coroutine_object() = default;
  coroutine_object(std::coroutine_handle<promise_type> co_handle) : co_handle{co_handle} {}
  coroutine_object(const coroutine_object& other) = delete;
  coroutine_object(coroutine_object&& other) noexcept : co_handle{other.co_handle} {
    other.co_handle = nullptr;
  }
  coroutine_object& operator=(const coroutine_object& other) = delete;
  coroutine_object& operator=(coroutine_object&& other) noexcept {
    co_handle = other.co_handle;
    other.co_handle = nullptr;
    return *this;
  }

  ~coroutine_object() {
    if (co_handle) {
      co_handle.destroy();
    }
  }

  void get() const {  // block
    co_handle.resume();
  }

  std::coroutine_handle<promise_type> co_handle;
};

template <typename T>
  requires(!std::is_same_v<T, void>) && std::is_move_assignable_v<T>
struct coroutine_object<T> {
  struct promise_type {
    std::optional<T> value;
    auto get_return_object() {
      return coroutine_object{std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    auto initial_suspend() {
      return std::suspend_never{};
    }
    auto final_suspend() noexcept {
      return std::suspend_always{};
    }
    void return_value(T value) {
      this->value = std::move(value);
    }
    void unhandled_exception() {}
  };

  coroutine_object() = default;
  coroutine_object(std::coroutine_handle<promise_type> co_handle) : co_handle{co_handle} {}
  coroutine_object(const coroutine_object& other) = delete;
  coroutine_object(coroutine_object&& other) noexcept : co_handle{other.co_handle} {
    other.co_handle = nullptr;
  }
  coroutine_object& operator=(const coroutine_object& other) = delete;
  coroutine_object& operator=(coroutine_object&& other) noexcept {
    co_handle = other.co_handle;
    other.co_handle = nullptr;
    return *this;
  }

  ~coroutine_object() {
    if (co_handle) {
      co_handle.destroy();
    }
  }

  std::optional<T> get() const {  // block
    co_handle.resume();
    return co_handle.promise().value;
  }

  std::coroutine_handle<promise_type> co_handle;
};

template <typename Suspend, typename Ready, typename Resume>
  requires requires(Suspend suspend, const Ready& ready, Resume resume) {
    { suspend() } -> std::same_as<void>;
    { ready() } -> std::same_as<bool>;
    { resume() } -> std::same_as<void>;
  }
struct detach_thread_awaitable : public Suspend, Ready, Resume {
  std::optional<guarded_thread> th;
  detach_thread_awaitable(Suspend suspend, Ready ready = Ready{}, Resume resume = Resume{})
      : Suspend{std::move(suspend)}, Ready{std::move(ready)}, Resume{std::move(resume)} {}
  bool await_ready() const noexcept {
    return Ready::operator()();
  }
  void await_suspend(std::coroutine_handle<> h) {
    th = guarded_thread{std::thread{[&]() { Suspend::operator()(); }}};
  }
  void await_resume() {
    if (th && (*th)->joinable()) {
      (*th)->join();
    }
    Resume::operator()();
  }
};

template <typename Suspend, typename Ready, typename Resume>
detach_thread_awaitable(Suspend, Ready, Resume) -> detach_thread_awaitable<Suspend, Ready, Resume>;

template <typename Suspend, typename Ready>
detach_thread_awaitable(Suspend, Ready)
    -> detach_thread_awaitable<Suspend, Ready, decltype([]() {})>;

template <typename Suspend>
detach_thread_awaitable(Suspend)
    -> detach_thread_awaitable<Suspend, decltype([]() { return false; }), decltype([]() {})>;

struct default_id_generator {
  size_t current_id{};
  size_t operator()() noexcept {
    return ++current_id;
  }
};

struct default_timestamp_generator {
  using time_point_t = std::chrono::time_point<std::chrono::system_clock>;
  time_point_t operator()() noexcept {
    return std::chrono::system_clock::now();
  }
};

enum class async_stream_read_write_status : std::uint8_t { empty, good };

template <typename T, std::invocable IDGen = default_id_generator,
          std::invocable TSGen = default_timestamp_generator>
  requires requires(T t) {
    t.serial_number = std::declval<IDGen>()();
    t.timestamp = std::declval<TSGen>()();
  } && std::is_default_constructible_v<IDGen> && std::is_default_constructible_v<TSGen>
class async_stream {
  using msg_t = T;

  IDGen id_gen{};
  TSGen ts_gen{};
  std::mutex mutex;
  stopable_cv cv;
  std::deque<T> queue;

 public:
  using status = async_stream_read_write_status;  // template params independent

  template <typename U>
    requires requires(U&& data) { msg_t{std::forward<U>(data)}; }
  status write_sync(U&& data) {
    msg_t msg{std::forward<U>(data)};
    {
      std::lock_guard lock{mutex};
      msg.serial_number = id_gen();
      msg.timestamp = ts_gen();
      queue.push_back(std::move(msg));
    }
    cv->notify_one();
    return status::good;
  }

  status read_sync(T& data) {
    std::unique_lock lock{mutex};
    cv->wait(lock, [this]() { return !queue.empty() || cv.is_stoped(); });
    if (!queue.empty()) {
      get_front(data);
      return status::good;
    }
    return status::empty;
  }

  operator bool() const noexcept {
    return !queue.empty() || !cv.is_stoped();
  }

  void stop() {
    cv.stop();
  }

 private:
  void get_front(T& data) {
    if constexpr (std::is_nothrow_move_assignable_v<T>) {  // move assign
      data = std::move(queue.front());
      queue.pop_front();
    } else if constexpr (std::is_nothrow_copy_assignable_v<T>) {  // copy assign
      data = queue.front();
      queue.pop_front();
    } else if constexpr (std::is_copy_constructible_v<T> &&
                         std::is_swappable_v<T>) {  // copy and swap
      T front(queue.front());
      std::swap(data, front);
      queue.pop_front();
    } else {  // data may be invalid when exceptions occur
      data = queue.front();
      queue.pop_front();
    }
  }
};

void try_message();

void try_async_stream();

void try_coroutine();

}  // namespace playground

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

template <typename Derived>
struct value_storage_base {
  std::exception_ptr e_ptr{nullptr};

  decltype(auto) get() & {
    if (e_ptr) {
      std::rethrow_exception(e_ptr);
    }
    return derived().get_impl();
  }

  decltype(auto) get() && {
    if (e_ptr) {
      std::rethrow_exception(e_ptr);
    }
    return std::move(derived()).get_impl();
  }

  template <typename F>
  void execute(F&& f) {
    try {
      derived().execute_impl(std::forward<F>(f));
    } catch (...) {
      e_ptr = std::current_exception();
    }
  }

 private:
  Derived& derived() {
    return static_cast<Derived&>(*this);
  }
};

template <typename T = void>
struct value_storage : public value_storage_base<value_storage<T>> {
  static_assert(!std::is_same_v<T, T>, "Invalid value_storage.");
  using type = T;

  void get_impl() noexcept {}
  template <typename F>
  void execute_impl(F&& f) {}
};

template <>
struct value_storage<void> : public value_storage_base<value_storage<void>> {
  using type = void;

  void get_impl() const noexcept {}
  template <std::invocable F>
  void execute_impl(F&& f) {
    std::forward<F>(f)();
  }
};

template <typename T>
struct value_storage<T&> : public value_storage_base<value_storage<T&>> {
  using type = T*;
  type value{nullptr};

  T& get_impl() const noexcept {
    return *value;
  }

  template <std::invocable F>
    requires std::convertible_to<std::invoke_result_t<F>, T&>
  void execute_impl(F&& f) {
    set(std::forward<F>(f)());
  }

  void set(T& _value) noexcept {
    value = std::addressof(_value);
  }
};

template <typename T>
  requires std::is_trivially_constructible_v<T>
struct value_storage<T> : public value_storage_base<value_storage<T>> {
  using type = T;
  type value{};

  T& get_impl() & noexcept {  // return T& instead of T, to avoid copy and support move
    return value;
  }

  T get_impl() && noexcept {
    return std::move(value);
  }

  template <std::invocable F>
    requires std::convertible_to<std::invoke_result_t<F>, T>
  void execute_impl(F&& f) {
    set(std::forward<F>(f)());
  }

  void set(T _value) noexcept(std::is_nothrow_move_assignable_v<T>) {
    value = std::move(_value);
  }
};

template <typename T>
  requires(!std::is_trivially_constructible_v<T>)
struct value_storage<T> : public value_storage_base<value_storage<T>> {
  using type = std::optional<T>;
  type value;

  T& get_impl() & noexcept {
    return value.value();
  }

  T get_impl() && noexcept {
    return std::move(value.value());
  }

  template <std::invocable F>
    requires std::convertible_to<std::invoke_result_t<F>, T>
  void execute_impl(F&& f) {
    set(std::forward<F>(f)());
  }

  void set(T _value) noexcept(std::is_nothrow_move_constructible_v<T> &&
                              std::is_nothrow_move_assignable_v<T>) {
    value = std::move(_value);
  }
};

template <typename Stream, typename T, typename Op>
concept stream_with_op = requires(Op op, Stream& s, T& data) { op(s, data); };

template <typename Stream, typename T>
concept istream_with = requires(Stream& is, T& data) { is >> data; };

template <typename Stream, typename T>
concept ostream_with = requires(Stream& os, T&& data) { os << std::forward<T>(data); };

struct read_stream {
  template <typename T>
  using data_type = T&;

  template <typename T, istream_with<T> Stream>
  auto operator()(Stream& is, T& data) {
    return is >> data;
  }
};

struct write_stream {
  template <typename T>
  using data_type = T;

  template <typename T, ostream_with<T> Stream>
  auto operator()(Stream& os, T&& data) {
    return os << std::forward<T>(data);
  }
};

template <typename T, typename Op, stream_with_op<T, Op> Stream, typename Executor>
class dispatcher {
  Stream& stream;
  Executor& executor;

 public:
  struct awaitable;

  dispatcher(Stream& _stream, Executor& _executor) : stream{_stream}, executor{_executor} {}

  template <typename U>
  awaitable operator()(U&& data) {
    return awaitable{.stream = stream, .executor = executor, .data = std::forward<U>(data)};
  }
};

template <typename T, typename Op, stream_with_op<T, Op> Stream>
struct to_execute_t {
  using data_t = Op::template data_type<T>;  // either T or T&

  Op* op;
  Stream* stream;
  T* data;

  using retval_t = decltype((*op)(*stream, *data));
  using storage_t = value_storage<retval_t>;

  storage_t* retval;
  std::coroutine_handle<> h;

  void operator()() {
    retval->execute([this]() { return (*op)(*stream, std::forward<data_t>(*data)); });
    h.resume();
  }

  void cancel() {
    if (h) {
      h.destroy();
    }
  }
};

template <typename T, typename Op, stream_with_op<T, Op> Stream, typename Executor>
struct dispatcher<T, Op, Stream, Executor>::awaitable {
  Stream& stream;
  Executor& executor;
  Op::template data_type<T> data;
  [[no_unique_address]] Op op{};

  using retval_t = decltype(op(stream, data));
  using storage_t = value_storage<retval_t>;
  [[no_unique_address]] storage_t retval;

  using _to_execute_t = to_execute_t<T, Op, Stream>;

  bool await_ready() const noexcept {
    return false;
  }

  void await_suspend(std::coroutine_handle<> h) {
    executor(_to_execute_t{.op = &op, .stream = &stream, .data = &data, .retval = &retval, .h = h});
  }

  retval_t await_resume() {
    return std::move(retval).get();
  }
};

template <typename T, istream_with<T> Stream, typename Executor>
using read_dispatcher = dispatcher<T, read_stream, Stream, Executor>;

template <typename T, ostream_with<T> Stream, typename Executor>
using write_dispatcher = dispatcher<T, write_stream, Stream, Executor>;

enum class sync_stream_read_write_status : std::uint8_t { empty, good };

template <std::movable T, std::invocable IDGen = default_id_generator,
          std::invocable TSGen = default_timestamp_generator>
  requires requires(T t) {
    t.serial_number = std::declval<IDGen>()();
    t.timestamp = std::declval<TSGen>()();
  } && std::is_default_constructible_v<IDGen> && std::is_default_constructible_v<TSGen>
class sync_stream {
  IDGen id_gen{};
  TSGen ts_gen{};
  std::mutex mutex;
  stopable_cv cv;
  std::deque<T> queue;

 public:
  using msg_t = T;
  using status = sync_stream_read_write_status;  // template params independent

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

  template <typename U>
    requires requires(U&& data) { msg_t{std::forward<U>(data)}; }
  status operator<<(U&& data) {
    return write_sync(std::forward<U>(data));
  }

  status operator>>(T& data) {
    return read_sync(data);
  }

  operator bool() const noexcept {
    return !queue.empty() || !cv.is_stoped();
  }

  void stop() {
    cv.stop();
  }

  template <typename Op, typename Executor>
  auto get_dispatcher(Op op, Executor& executor) {
    return dispatcher<msg_t, Op, sync_stream, Executor>{*this, executor};
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
      data = std::move(queue.front());
      queue.pop_front();
    }
  }
};

template <typename T>
concept cancellable = requires(T obj) { obj.cancel(); };

template <std::movable F>
class runner {
  toyqueue::fix_cap_queue<F> queue;
  std::stop_source stop_source;
  std::counting_semaphore<> semaphore{0};
  guarded_thread th;

  void stop() {
    stop_source.request_stop();
    semaphore.release();
  }

  void drain() {
    if constexpr (cancellable<F>) {
      while (!queue.empty()) {
        auto task = queue.try_pop();
        if (task.has_value()) {
          task.value().cancel();
        }
      }
    } else {
    }
  }

  void run() {
    auto stop_token = stop_source.get_token();
    while (true) {
      semaphore.acquire();
      if (stop_token.stop_requested()) {
        drain();
        return;
      }
      auto task = queue.try_pop();
      if (task.has_value()) {
        task.value()();
      }
    }
  }

 public:
  runner(size_t log_cap = 16) : queue{log_cap}, th{std::thread{[this]() { run(); }}} {}

  ~runner() {
    stop();
  }

  template <typename U>
  void operator()(U&& task) {
    F f{std::forward<U>(task)};
    while (!queue.try_push(std::move(f))) {
      std::this_thread::yield();
    }
    semaphore.release();
  }
};

struct done_mixin {
  std::shared_ptr<std::binary_semaphore> done = std::make_shared<std::binary_semaphore>(0);
  void _return() const {
    done->release();
  }
};

struct return_void_mixin : public done_mixin {
  using base = done_mixin;
  [[no_unique_address]] value_storage<void> retval;
  void return_void() const {
    base::_return();
  }
};

template <typename T>
struct return_value_mixin : public done_mixin {
  using base = done_mixin;
  [[no_unique_address]] value_storage<T> retval;
  void return_value(T value) {
    retval.set(std::forward<T>(value));
    base::_return();
  }
};

template <typename T>
using return_mixin =
    std::conditional_t<std::is_same_v<T, void>, return_void_mixin, return_value_mixin<T>>;

template <typename T>
struct co_task_awaitable;

template <typename T = void>
struct co_task_with {
  struct final_awaitable {
    std::coroutine_handle<> next{nullptr};
    bool await_ready() const noexcept {
      return !next;
    }
    auto await_suspend(std::coroutine_handle<> h) const noexcept {
      return next;
    }
    void await_resume() const noexcept {}
  };

  struct promise_type : public return_mixin<T> {
    std::coroutine_handle<> next{nullptr};
    auto get_return_object() {
      return co_task_with{.co_handle = std::coroutine_handle<promise_type>::from_promise(*this),
                          .done = this->done,
                          .promise = *this};
    }
    auto initial_suspend() {
      return std::suspend_always{};
    }
    auto final_suspend() noexcept {
      return final_awaitable{next};
    }
    void unhandled_exception() {}
  };

  co_task_with& launch() & {
    co_handle.resume();
    return *this;
  }

  co_task_with launch() && {
    co_handle.resume();
    return std::move(*this);
  }

  co_task_awaitable<T> wait() & {
    return co_task_awaitable<T>{*this};
  }

  void sync_wait() const {
    done->acquire();
  }

  void hook_next(std::coroutine_handle<> h) noexcept {
    promise.next = h;
  }

  std::coroutine_handle<promise_type> co_handle;
  std::shared_ptr<std::binary_semaphore> done;
  promise_type& promise;
};

template <typename T>
struct co_task_awaitable {
  using task_t = co_task_with<T>;
  task_t& task;

  bool await_ready() const noexcept {
    return false;
  }
  auto await_suspend(std::coroutine_handle<> h) const noexcept {
    task.hook_next(h);
    return task.co_handle;  // transfer to run co_handle.resume();
  }
  T await_resume() const noexcept {
    value_storage<T> value{std::move(task.promise.retval)};
    task.co_handle.destroy();
    return std::move(value).get();
  }
};

using co_task = co_task_with<>;

struct cancel_mixin {
  std::function<void()> _cancel{[]() {}};
  void cancel() const {
    _cancel();
  }
};

template <typename R, typename... Args>
struct call_mixin {
  std::function<R(Args...)> call;
  R operator()(Args... args) {
    return call(std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args>
struct cancellable_function : public cancel_mixin, call_mixin<R, Args...> {
  using cancel_base = cancel_mixin;
  using call_base = call_mixin<R, Args...>;

  template <typename F, typename C>
  cancellable_function(F func, C cancel)
      : cancel_base{std::move(cancel)}, call_base{std::move(func)} {}

  template <typename F>
  cancellable_function(F func) : cancel_base{}, call_base{std::move(func)} {}
};

template <typename T, typename Op, stream_with_op<T, Op> Stream>
auto to_function(to_execute_t<T, Op, Stream> to_execute) {
  std::coroutine_handle<> h = to_execute.h;
  return cancellable_function<void>([f = std::move(to_execute)]() { f(); },  // execute
                                    [h]() {                                  // cancel
                                      if (h) {
                                        h.destroy();
                                      }
                                    });
}

template <typename Executor>
struct execute_by_awaitable {
  Executor& executor;
  bool await_ready() const noexcept {
    return false;
  }
  void await_suspend(std::coroutine_handle<> h) noexcept {
    executor(cancellable_function<void> {
      [=]() { h.resume(); },
      [=]() { h.destroy(); }
    });
  }
  void await_resume() const noexcept {}
};

template <typename Executor>
auto execute_by(Executor& executor) {
  return execute_by_awaitable{executor};
}

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

}  // namespace playground

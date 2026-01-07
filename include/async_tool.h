#pragma once

#include <algorithm>
#include <concepts>
#include <coroutine>
#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <semaphore>
#include <type_traits>
#include <utility>

namespace async {

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

template <typename T>
concept cancellable = requires(T obj) { obj.cancel(); };

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
    executor(cancellable_function<void>{[=]() { h.resume(); }, [=]() { h.destroy(); }});
  }
  void await_resume() const noexcept {}
};

template <typename Executor>
auto execute_by(Executor& executor) {
  return execute_by_awaitable{executor};
}

};  // namespace async

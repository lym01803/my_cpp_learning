#include <playground.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <locale>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <queue>
#include <random>
#include <ranges>
#include <ratio>
#include <shared_mutex>
#include <stdexcept>
#include <stop_token>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "message.h"
#include "toyqueue.h"

namespace playground {

void try_concurrency() {
  auto f = [](std::string s) { std::cout << s << std::endl; };
  guarded_thread t{std::thread{f, "launch"}};

  std::vector<std::string> strings{"Hello", ",", " ", "world", "!", "\n"};
  std::vector<guarded_thread> threads;
  threads.reserve(strings.size());
  for (const auto& str : strings) {
    threads.emplace_back(std::thread{[](const std::string& s) { std::cout << s; }, std::ref(str)});
  }
  for (auto& t : threads) {
    if (t->joinable()) {
      t->join();
    }
  }
  std::cout << std::endl;

  std::cout << std::format("hardware_concurrency: {}", std::thread::hardware_concurrency())
            << std::endl;

  auto* try_release = new guarded_thread{std::thread{[]() {
    std::cout << "try release" << std::endl;
    std::chrono::duration<long long, std::milli> duration{3000};
    std::this_thread::sleep_for(duration);
    std::cout << "after sleep for " << duration.count() << " ms" << std::endl;
  }}};
  auto t2 = try_release->release();
  delete try_release;
  t2.join();
}

namespace {

void sum_and_add_without_lock() {
  const long long N = 1'000'000'000;
  std::vector nums{std::from_range, std::views::iota(1LL, N)};
  long long result{};

  guarded_thread calc_sum{std::thread{[](const decltype(nums)& vec, long long& result) {
                                        long long sum = 0LL;
                                        for (const auto& item : vec) {
                                          sum += item;
                                        }
                                        result = sum;
                                      },
                                      std::ref(nums), std::ref(result)}};

  guarded_thread modify_vec{std::thread{[](decltype(nums)& vec) {
                                          for (auto& item : vec) {
                                            item += 1LL;
                                          }
                                        },
                                        std::ref(nums)}};

  calc_sum->join();
  modify_vec->join();
  std::cout << "sum result = " << result << std::endl;
}

void sum_and_add_with_lock() {
  const long long N = 1'000'000'000;
  std::vector nums{std::from_range, std::views::iota(1LL, N)};
  long long result{};
  std::mutex mutex;

  guarded_thread calc_sum{std::thread{[&mutex](const decltype(nums)& vec, long long& result) {
                                        long long sum = 0LL;
                                        {
                                          std::scoped_lock lock{mutex};
                                          for (const auto& item : vec) {
                                            sum += item;
                                          }
                                        }
                                        result = sum;
                                      },
                                      std::ref(nums), std::ref(result)}};

  guarded_thread modify_vec{std::thread{[&mutex](decltype(nums)& vec) {
                                          std::scoped_lock lock{mutex};
                                          for (auto& item : vec) {
                                            item += 1LL;
                                          }
                                        },
                                        std::ref(nums)}};

  calc_sum->join();
  modify_vec->join();
  std::cout << "sum result = " << result << std::endl;
}

}  // namespace

void try_mutex() {
  for (int i = 0; i < 3; i++) {
    std::cout << std::format("time: {}", timer_wrap(sum_and_add_without_lock)()) << std::endl;
    std::cout << std::format("time: {}", timer_wrap(sum_and_add_with_lock)()) << std::endl;
  }
}

void play_with_matrix() {
  Matrix2D<int> m{3, 4};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      m[i, j] = (i + 1) * (j + 1);
    }
  }
  Matrix2D<int> m2(m);
  m2[2, 3] = -1;
  std::cout << m << std::endl;
  std::cout << m2 << std::endl;
  Matrix2D<double> n(m);
  n[0, 0] = 3.14;
  std::cout << n << std::endl;
}

void try_shared_mutex() {
  std::shared_mutex s_mutex;
  const long long range_max_value = 300'000'000LL;
  std::vector arr{std::from_range, std::views::iota(1LL, range_max_value + 1LL)};
  std::mt19937_64 gen{std::random_device{}()};
  const int max_delay_miliseconds = 30;
  std::uniform_int_distribution dist(0, max_delay_miliseconds);

  auto sum_with_shared_mutex = [](const auto& data, std::shared_mutex& s_mutex) {
    std::shared_lock<std::shared_mutex> lock{s_mutex};
    return std::ranges::fold_left(data, 0LL, [](long long s, long long item) { return s + item; });
  };
  auto add_with_shared_mutex = [](auto& data, std::shared_mutex& s_mutex) {
    std::unique_lock<std::shared_mutex> lock{s_mutex};
    std::ranges::for_each(data, [](long long& item) { item += 1; });
  };

  auto reader_task = [&](std::string tag) {
    auto delay = dist(gen);
    std::this_thread::sleep_for(std::chrono::milliseconds{delay});
    std::cout << std::format("{} starts after {} ms delay.\n", tag, delay) << std::endl;
    auto res = sum_with_shared_mutex(arr, s_mutex);
    std::cout << std::format("{} sum: {}\n", tag, res) << std::endl;
  };
  auto writer_task = [&](std::string tag) {
    auto delay = dist(gen);
    std::this_thread::sleep_for(std::chrono::milliseconds{delay});
    std::cout << std::format("{} starts after {} ms delay.\n", tag, delay) << std::endl;
    add_with_shared_mutex(arr, s_mutex);
    std::cout << std::format("{} finished writing\n", tag) << std::endl;
  };

  auto tagged_timer_wrap = []<typename F>(std::string tag, F f) {
    return [tag = std::move(tag), f = std::move(f)]() {
      auto time = timer_wrap([&]() { f(tag); })();
      std::cout << std::format("{} cost time: {}\n", tag, time) << std::endl;
    };
  };

  tagged_timer_wrap("total", [&](std::string tag) {
    guarded_thread reader_1{std::thread{tagged_timer_wrap("reader 1", reader_task)}};
    guarded_thread reader_2{std::thread{tagged_timer_wrap("reader 2", reader_task)}};
    guarded_thread writer{std::thread{tagged_timer_wrap("writer", writer_task)}};
  })();
}

void try_condition_variable() {
  std::condition_variable cv;
  std::optional<int> x{};
  std::mutex data_mutex;

  guarded_thread reader{std::thread{[&]() {
    std::unique_lock lock{data_mutex};
    const auto d = std::chrono::milliseconds{1000};
    for (int i = 0; i < 5; i++) {
      if (cv.wait_for(lock, d, [&]() { return x.has_value(); })) {
        std::cout << std::format("i = {}, x = {}\n", i, *x);
        x = std::nullopt;
      }
    }
  }}};

  guarded_thread writer{std::thread{[&]() {
    const auto d = std::chrono::milliseconds{500};
    for (int i = 0; i < 5; i++) {
      // 模拟耗时
      std::this_thread::sleep_for(d);
      {
        std::lock_guard lock{data_mutex};
        x = i;
        cv.notify_all();
      }
    }
  }}};
}

void try_condition_variable_with_stop() {  // NOLINT(readability-function-cognitive-complexity)
  const auto N = 1000'000'000LL;
  std::vector data{std::from_range, std::views::iota(1LL, N + 1LL)};
  std::queue<long long> queue;
  std::mutex mutex;
  stopable_cv cv;

  auto fetch_and_output = [](auto& queue) {
    while (!queue.empty()) {
      const auto front = queue.front();
      queue.pop();
      std::cout << front << " ";
    }
    std::cout << std::endl;
  };

  guarded_thread reader{std::thread{[&]() {
    std::unique_lock lock{mutex};
    while (true) {
      cv->wait(lock, [&]() { return !queue.empty() || cv.is_stoped(); });
      if (!queue.empty()) {
        fetch_and_output(queue);
      } else {
        break;
      }
    }
  }}};

  guarded_thread writer{std::thread{[&]() {
    for (int i = 0; i < 5; i++) {
      long long sum = std::ranges::fold_left(data, 0LL, std::plus{});
      {
        std::lock_guard lock{mutex};
        queue.push(sum);
        cv->notify_one();
      }
    }
    cv.stop();
  }}};
}

namespace {

template <typename... Ts>
struct temp_visitor : public Ts... {
  using Ts::operator()...;
};

template <typename... Ts>
temp_visitor(Ts...) -> temp_visitor<Ts...>;  // pass value

class arr_msg {
  std::shared_ptr<std::vector<int>> data_ptr;

 public:
  arr_msg() = default;
  arr_msg(std::shared_ptr<std::vector<int>> ptr) : data_ptr{std::move(ptr)} {}
  std::vector<int>& data() noexcept {  // NOLINT(readability-make-member-function-const)
    return *data_ptr;
  }
  const std::vector<int>& data() const noexcept {
    return *data_ptr;
  }
};

}  // namespace

void try_message() {
  using msg_v_t = std::variant<std::monostate, double, int, std::string, char, arr_msg,
                               std::pair<double, double>>;
  using msg_t = msg::message<msg_v_t>;
  std::vector<msg_t> messages;
  messages.reserve(7);

  messages.emplace_back(3.14, 1);
  messages.emplace_back(12345, 2);
  messages.emplace_back("Hello, world!", 3);
  messages.emplace_back('c', 4);
  auto arr_ptr = std::make_shared<std::vector<int>>(std::initializer_list{-1, 0, 1});
  messages.emplace_back(arr_msg{std::move(arr_ptr)}, 5);
  messages.emplace_back(std::make_pair(2.71828, 3.14159), 6);
  messages.emplace_back(msg_t{.serial_number = 7});

  auto inplace_modify = temp_visitor{[](auto& data) { /* fallback, do nothing */ },
                                     [](std::string& data) { data += "[suffix]"; },
                                     [](arr_msg& data) {
                                       auto& arr = data.data();
                                       const auto size = arr.size();
                                       // append #size numbers
                                       for (size_t i = 0; i < size; i++) {
                                         arr.emplace_back(1 + *arr.rbegin());
                                       }
                                     }};

  auto output = temp_visitor{[](const auto& data) { /* fallback, do nothing */ },
                             [](double data) { std::cout << data << std::endl; },
                             [](int data) { std::cout << data << std::endl; },
                             [](const std::string& data) { std::cout << data << std::endl; },
                             [](char data) { std::cout << data << std::endl; },
                             [](const arr_msg& data) {
                               const auto& arr = data.data();
                               for (const auto& item : arr) {
                                 std::cout << item << " ";
                               }
                               std::cout << std::endl;
                             },
                             [](const std::pair<double, double>& data) {
                               std::cout << std::format("({}, {})", data.first, data.second)
                                         << std::endl;
                             }};

  std::ranges::sort(messages, std::greater<msg_t>{});  // sort by serial number in descending order
  // here filter is unnecessary, since visitor has fallback branch. only to show the usage of
  // msg_t::data_t_in<>();
  std::ranges::for_each(messages | std::views::filter([](const msg_t& m) {
                          return m.data_t_in<std::string, arr_msg>();
                        }),
                        [&](msg_t& m) { std::visit(std::move(inplace_modify), m.data); });
  std::ranges::for_each(messages, [&](const msg_t& m) { std::visit(std::move(output), m.data); });
}

namespace {

struct counter_controller {
  std::atomic<int> count{0};
  std::function<void()> callback;
  operator int() const noexcept {
    return count;
  }
  counter_controller& operator++() noexcept {
    ++count;
    return *this;
  }
  counter_controller& operator--() {
    if (--count == 0) {
      callback();
    }
    return *this;
  }
  struct guard {
    counter_controller& ctrl;
    guard(counter_controller& ctrl) noexcept : ctrl{ctrl} {
      ++ctrl;
    }
    ~guard() {
      --ctrl;
    }
  };
};

struct sum_msg {
  size_t value;
};

struct square_sum_msg {
  size_t value;
};

}  // namespace

void try_msg_stream() {
  const auto N = 100'000'000LL;
  const size_t mod = 998244353;
  std::vector data{std::from_range, std::views::iota(1LL, N + 1LL)};

  using msg_t = msg::message<std::variant<std::monostate, sum_msg, square_sum_msg>>;
  sync_stream<msg_t> stream;
  counter_controller counter{.callback = [&]() { stream.stop(); }};

  guarded_thread task1{std::thread{[&]() {
    counter_controller::guard guard{counter};
    for (int i = 0; i < 5; i++) {
      stream.write_sync(sum_msg{std::ranges::fold_left(data, (size_t)0, std::plus<size_t>{})});
    }
  }}};

  guarded_thread task2{std::thread{[&]() {
    counter_controller::guard guard{counter};
    for (int i = 0; i < 5; i++) {
      stream.write_sync(
          square_sum_msg{std::ranges::fold_left(data, (size_t)0, [](size_t s, size_t v) {
            auto res = s + (v * v % mod);
            res = res >= mod ? res - mod : res;
            res = res < 0 ? res + mod : res;
            return res;
          })});
    }
  }}};

  auto output =
      temp_visitor{[](const auto& data) {},
                   [](const sum_msg& data) { std::cout << "sum = " << data.value << std::endl; },
                   [](const square_sum_msg& data) {
                     std::cout << "moded square sum = " << data.value << std::endl;
                   }};
  guarded_thread output_task{std::thread{[&]() {
    while (stream) {
      msg_t msg;
      if (stream.read_sync(msg) == sync_stream<msg_t>::status::good) {
        std::visit(output, msg.data);
      }
    }
  }}};
}

void try_coroutine() {}

void try_toy_queue() {
  using queue_t = toyqueue::fix_cap_queue<std::string>;
  const size_t log_cap = 10;
  const size_t num = 1000;
  queue_t queue{log_cap};
  std::stop_source stop_signal;
  counter_controller counter{.callback = [&]() { stop_signal.request_stop(); }};

  auto product = [&](std::string tag) {
    counter_controller::guard ctrl_gd{counter};
    for (int i = 0; i < num; i++) {
      while (!queue.try_push(std::format("{}: {}", tag, i)));
    }
  };

  auto consume = [&](std::string tag, std::stop_token stop) {
    while (!stop.stop_requested() ||
           !queue.empty()) {  // when stop, no more push, the value of !queue.empty() can only
                              // change from true to false between two evaluations;
      std::optional<queue_t::value_t> data;
      data = queue.try_pop();
      if (data.has_value()) {
        std::cout << std::format("{}: data: {}\n", tag, data.value());
      }
    }
    std::cout << std::flush;
  };

  guarded_thread p1{std::thread{product, "p1"}};
  guarded_thread p2{std::thread{product, "p2"}};
  guarded_thread p3{std::thread{product, "p3"}};
  guarded_thread p4{std::thread{product, "p4"}};
  guarded_thread c1{std::thread{consume, "c1", stop_signal.get_token()}};
  guarded_thread c2{std::thread{consume, "c2", stop_signal.get_token()}};
}

namespace {

struct toy_queue_test {
  using queue_t = toyqueue::fix_cap_queue<size_t>;

  const size_t log_cap{20};
  const size_t num{1'000'000};
  queue_t queue{log_cap};
  std::vector<size_t> temp_retvals;
  std::mutex mutex_for_temp_retvals;

  void product(std::shared_ptr<counter_controller> counter) {
    counter_controller::guard counter_gd{*counter};
    for (size_t i = 0; i < num; i++) {
      while (!queue.try_push(1)) {
        std::this_thread::yield();
      }
    }
  }

  void product_serial() {
    for (size_t i = 0; i < num; i++) {
      queue.try_push(1);
    }
  }

  size_t consume(std::stop_token stop) {
    size_t sum = 0;
    while (!stop.stop_requested() || !queue.empty()) {
      auto data = queue.try_pop();
      if (data.has_value()) {
        sum += data.value();
      } else {
        std::this_thread::yield();
      }
    }
    return sum;
  }

  size_t consume_serial() {
    size_t sum = 0;
    while (!queue.empty()) {
      auto data = queue.try_pop();
      if (data.has_value()) {
        sum += data.value();
      }
    }
    return sum;
  }

  template <typename F>
  auto tagged_output_wrap(std::string tag, F f) {
    return [tag = std::move(tag), f = std::move(f), this]() mutable {
      size_t retval = f();
      {
        std::lock_guard lock{mutex_for_temp_retvals};
        temp_retvals.emplace_back(retval);
      }
      std::cout << std::format("{} return value {}\n", tag, retval) << std::endl;
    };
  }

  template <typename F>
  auto tagged_timer_wrap(std::string tag, F f) {
    return [tag = std::move(tag), f = timer_wrap(std::move(f))]() mutable {
      auto time = f();
      std::cout << std::format("{} cost time {}\n", tag,
                               std::chrono::duration_cast<std::chrono::milliseconds>(time))
                << std::endl;
    };
  }

  /** @warning do not run test_concurrent concurrently */
  void test_concurrent(size_t num_producer = 1, size_t num_consumer = 1) {
    temp_retvals.clear();
    std::stop_source stop;
    auto counter = std::make_shared<counter_controller>((size_t)0, [&]() { stop.request_stop(); });

    tagged_timer_wrap("[concurrent total]", [&]() {
      std::vector<guarded_thread> threads;
      for (size_t i = 0; i < num_producer; i++) {
        auto tag = std::format("[producer {}]", i);
        threads.emplace_back(std::thread{tagged_timer_wrap(tag, [&]() { product(counter); })});
      }
      for (size_t i = 0; i < num_consumer; i++) {
        auto tag = std::format("[consumer {}]", i);
        threads.emplace_back(std::thread{tagged_timer_wrap(
            tag, tagged_output_wrap(tag, [&]() { return consume(stop.get_token()); }))});
      }
    })();

    std::cout << std::format("[total count] {}",
                             std::ranges::fold_left(temp_retvals, (size_t)0, std::plus<size_t>{}))
              << std::endl;
  }

  /** @warning do not run test_concurrent concurrently */
  void test_concurrent_with_initial_data(size_t num_producer = 1, size_t num_consumer = 1,
                                         size_t initial_data_num = -1) {
    if (initial_data_num == -1) {
      initial_data_num = num * num_producer / 2;
    }
    for (size_t i = 0; i < initial_data_num; i++) {
      queue.try_push(1);
    }

    temp_retvals.clear();
    std::stop_source stop;
    auto counter = std::make_shared<counter_controller>((size_t)0, [&]() { stop.request_stop(); });

    tagged_timer_wrap("[concurrent total]", [&]() {
      std::vector<guarded_thread> threads;
      for (size_t i = 0; i < num_producer; i++) {
        auto tag = std::format("[producer {}]", i);
        threads.emplace_back(std::thread{tagged_timer_wrap(tag, [&]() { product(counter); })});
      }
      for (size_t i = 0; i < num_consumer; i++) {
        auto tag = std::format("[consumer {}]", i);
        threads.emplace_back(std::thread{tagged_timer_wrap(
            tag, tagged_output_wrap(tag, [&]() { return consume(stop.get_token()); }))});
      }
    })();

    std::cout << std::format("[total count] {}",
                             std::ranges::fold_left(temp_retvals, (size_t)0, std::plus<size_t>{}))
              << std::endl;
  }

  void test_serial() {
    tagged_timer_wrap("[serial total]", [&]() {
      tagged_timer_wrap("[producer]", [this]() { product_serial(); })();
      tagged_timer_wrap("[consumer]",
                        tagged_output_wrap("[consumer]", [&]() { return consume_serial(); }))();
    })();
  }

  void naive_sum() {
    tagged_timer_wrap("[naive sum]", [&]() {
      std::vector<size_t> arr;
      arr.reserve(num);
      for (int i = 0; i < num; i++) {
        arr.emplace_back(1);
      }
      std::cout << std::format("[naive sum] count {}",
                               std::ranges::fold_left(arr, (size_t)0, std::plus<size_t>{}))
                << std::endl;
    })();
  }
};

struct mutex_queue_test {
  using queue_t = toyqueue::naive_fix_cap_queue<size_t>;

  const size_t cap{0x100000};
  const size_t num{1'000'000};
  queue_t queue{cap};

  void product(std::shared_ptr<counter_controller> counter, std::mutex& mutex, stopable_cv& cv) {
    counter_controller::guard counter_gd{*counter};
    for (size_t i = 0; i < num; i++) {
      std::unique_lock lock{mutex};
      while (queue.full()) {
        lock.unlock();
        std::this_thread::yield();
        lock.lock();
      }
      queue.push(1);
      lock.unlock();
      cv->notify_one();
    }
  }

  size_t consume(std::mutex& mutex, stopable_cv& cv) {
    size_t sum = 0;
    while (true) {
      std::unique_lock lock{mutex};
      cv->wait(lock, [&]() { return !queue.empty() || cv.is_stoped(); });
      if (!queue.empty()) {
        auto data = queue.pop();
        lock.unlock();
        if (data.has_value()) {
          sum += data.value();
        }
      } else {
        break;
      }
    }
    return sum;
  }

  template <typename F>
  auto tagged_output_wrap(std::string tag, F f) {
    return [tag = std::move(tag), f = std::move(f), this]() mutable {
      size_t retval = f();
      std::cout << std::format("{} return value {}\n", tag, retval) << std::endl;
    };
  }

  template <typename F>
  auto tagged_timer_wrap(std::string tag, F f) {
    return [tag = std::move(tag), f = timer_wrap(std::move(f))]() mutable {
      auto time = f();
      std::cout << std::format("{} cost time {}\n", tag,
                               std::chrono::duration_cast<std::chrono::milliseconds>(time))
                << std::endl;
    };
  }

  /** @warning do not run test_concurrent concurrently */
  void test_concurrent(size_t num_producer = 1, size_t num_consumer = 1) {
    std::mutex mutex;
    stopable_cv cv;
    auto counter = std::make_shared<counter_controller>((size_t)0, [&]() { cv.stop(); });

    tagged_timer_wrap("[concurrent total]", [&]() {
      std::vector<guarded_thread> threads;
      for (size_t i = 0; i < num_producer; i++) {
        auto tag = std::format("[producer {}]", i);
        threads.emplace_back(
            std::thread{tagged_timer_wrap(tag, [&]() { product(counter, mutex, cv); })});
      }
      for (size_t i = 0; i < num_consumer; i++) {
        auto tag = std::format("[consumer {}]", i);
        threads.emplace_back(std::thread{
            tagged_timer_wrap(tag, tagged_output_wrap(tag, [&]() { return consume(mutex, cv); }))});
      }
    })();
  }

  /** @warning do not run test_concurrent concurrently */
  void test_concurrent_with_initial_data(size_t num_producer = 1, size_t num_consumer = 1,
                                         size_t initial_data_num = -1) {
    if (initial_data_num == -1) {
      initial_data_num = num * num_producer / 2;
    }
    for (size_t i = 0; i < initial_data_num; i++) {
      if (!queue.full()) {
        queue.push(1);
      }
    }

    std::mutex mutex;
    stopable_cv cv;
    auto counter = std::make_shared<counter_controller>((size_t)0, [&]() { cv.stop(); });

    tagged_timer_wrap("[concurrent total]", [&]() {
      std::vector<guarded_thread> threads;
      for (size_t i = 0; i < num_producer; i++) {
        auto tag = std::format("[producer {}]", i);
        threads.emplace_back(
            std::thread{tagged_timer_wrap(tag, [&]() { product(counter, mutex, cv); })});
      }
      for (size_t i = 0; i < num_consumer; i++) {
        auto tag = std::format("[consumer {}]", i);
        threads.emplace_back(std::thread{
            tagged_timer_wrap(tag, tagged_output_wrap(tag, [&]() { return consume(mutex, cv); }))});
      }
    })();
  }
};

}  // namespace

void try_toy_queue2() {
  const size_t times = 10;
  const size_t N = 1'000'000;
  const size_t log_cap = 20;
  // naive_sum
  std::cout << "==============================================" << "naive_sum"
            << "==============================================" << std::endl;
  {
    toy_queue_test test{.num = 4 * N};
    for (size_t i = 0; i < times; i++) {
      test.naive_sum();
    }
  }
  // serial
  std::cout << "==============================================" << "serial"
            << "==============================================" << std::endl;
  {
    toy_queue_test test{.log_cap = 2 + log_cap, .num = 4 * N};
    for (size_t i = 0; i < times; i++) {
      test.test_serial();
    }
  }
  // concurrency, spsc
  std::cout << "==============================================" << "spsc"
            << "==============================================" << std::endl;
  {
    toy_queue_test test{.log_cap = 2 + log_cap, .num = 4 * N};
    for (size_t i = 0; i < times; i++) {
      test.test_concurrent(1, 1);
    }
  }
  // concurrency, naive queue + mutex / cv, spsc
  std::cout << "=============================================="
            << "naive queue + mutex / cv, spsc"
            << "==============================================" << std::endl;
  {
    mutex_queue_test test{.cap = 4 * N + 1, .num = 4 * N};
    for (size_t i = 0; i < times; i++) {
      test.test_concurrent(1, 1);
    }
  }
  // concurrency, mpmc absolutely sufficient cap
  std::cout << "=============================================="
            << "mpmc (4p2c) with absolutely sufficient cap"
            << "==============================================" << std::endl;
  {
    toy_queue_test test{.log_cap = 2 + log_cap, .num = N};
    for (size_t i = 0; i < times; i++) {
      test.test_concurrent(4, 2);
    }
  }
  // concurrency, naive queue + mutex / cv, mpmc
  std::cout << "=============================================="
            << "naive queue + mutex / cv, mpmc (4p2c)"
            << "==============================================" << std::endl;
  {
    mutex_queue_test test{.cap = 4 * N + 1, .num = N};
    for (size_t i = 0; i < times; i++) {
      test.test_concurrent(4, 2);
    }
  }
  // concurrency, mpmc absolutely sufficient cap + half initial data
  std::cout << "=============================================="
            << "mpmc (4p2c) with absolutely sufficient cap + 1/4 initial data"
            << "==============================================" << std::endl;
  {
    toy_queue_test test{.log_cap = 2 + log_cap, .num = N};
    for (size_t i = 0; i < times; i++) {
      test.test_concurrent_with_initial_data(4, 2, N);
    }
  }
  // concurrency, naive queue + mutex / cv, mpmc + half initial data
  std::cout << "=============================================="
            << "naive queue + mutex / cv, mpmc (4p2c) + 1/4 initial data"
            << "==============================================" << std::endl;
  {
    mutex_queue_test test{.cap = 4 * N + 1, .num = N};
    for (size_t i = 0; i < times; i++) {
      test.test_concurrent_with_initial_data(4, 2, N);
    }
  }
  // concurrency, mpmc with relatively sufficient cap
  std::cout << "=============================================="
            << "mpmc (4p2c) with relatively sufficient cap"
            << "==============================================" << std::endl;
  {
    toy_queue_test test{.log_cap = 16, .num = N};
    for (size_t i = 0; i < times; i++) {
      test.test_concurrent(4, 2);
    }
  }
  // concurrency, mpmc insufficient cap
  std::cout << "=============================================="
            << "mpmc (4p2c) with insufficient cap"
            << "==============================================" << std::endl;
  {
    toy_queue_test test{.log_cap = 4, .num = N};
    for (size_t i = 0; i < times; i++) {
      test.test_concurrent(4, 2);
    }
  }
  // concurrency, mpmc extremely insufficient cap
  std::cout << "=============================================="
            << "mpmc (4p2c) with extremely insufficient cap"
            << "==============================================" << std::endl;
  {
    toy_queue_test test{.log_cap = 0, .num = N};
    for (size_t i = 0; i < times; i++) {
      test.test_concurrent(4, 2);
    }
  }
}

namespace {

template <typename F, typename... Caps>
struct toy_lambda {
  template <typename T>
  struct index_helper;

  template <size_t... Is>
  struct index_helper<std::index_sequence<Is...>> {
    template <typename... Args>
    static auto call(F& f, std::tuple<Caps...>& captures, Args&&... args) {
      return std::invoke(f, std::get<Is>(captures)..., std::forward<Args>(args)...);
    }
  };

  using index_t = decltype(std::make_index_sequence<sizeof...(Caps)>());

  F f;
  std::tuple<Caps...> captures;
  toy_lambda(F f, Caps... caps) : f(std::forward<F>(f)), captures{std::forward<Caps>(caps)...} {}

  template <typename... Args>
    requires std::invocable<F, Caps..., Args...>
  auto operator()(Args&&... args) {
    return index_helper<index_t>::call(f, captures, std::forward<Args>(args)...);
  }
};

template <typename F, typename... Caps>
toy_lambda(F, Caps...) -> toy_lambda<F, Caps...>;

// int add(int x, int y) {
//   return x + y;
// }

}  // namespace

void try_toy_duck_type() {
  int x = 10;
  int y = 20;

  struct add {
    auto operator()(int x, int y) const {
      return x + y;
    }
  };

  auto f = toy_lambda(add{});  // 不捕获
  std::cout << f(x, y) << std::endl;
  auto g = toy_lambda(add{}, x, y);  // 值捕获
  auto g2 = toy_lambda(add{}, x, y);
  static_assert(std::is_same_v<decltype(g), decltype(g2)>, "not same type");
  std::cout << g() << std::endl;
  auto h = toy_lambda(add{}, std::ref(x), std::ref(y));  // "引用"捕获
  std::cout << h() << std::endl;
  auto k = toy_lambda<add, int&, int&>(add{}, x, y);  // 真-引用捕获
  std::cout << k() << std::endl;
}

void try_await() {
  using msg_t = msg::message<std::variant<std::monostate, int, double>>;
  using stream_t = sync_stream<msg_t>;

  std::atomic<double> sum{0.0};

  stream_t stream;
  auto read_worker = runner<to_execute_t<msg_t, read_stream, stream_t>>();
  auto write_worker = runner<to_execute_t<msg_t, write_stream, stream_t>>();
  auto reader = stream.get_dispatcher(read_stream{}, read_worker);
  auto writer = stream.get_dispatcher(write_stream{}, write_worker);

  guarded_thread writer1{std::thread{[&]() {
    for (size_t i = 0; i < 50; i++) {
      [&]() -> co_task { co_await writer(1); }().launch();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }}};

  guarded_thread writer2{std::thread{[&]() {
    for (size_t i = 0; i < 200; i++) {
      [&]() -> co_task { co_await writer(0.25); }().launch();
      std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
  }}};

  auto t = timer_wrap([&]() {
    for (size_t i = 0; i < 250; i++) {
      [&]() -> co_task {
        msg_t msg;
        co_await reader(msg);
        if (msg) {
          std::visit(temp_visitor{[&](int value) { sum += (double)value; },
                                  [&](double value) { sum += value; }, [&](auto& value) {}},
                     msg.data);
        }
      }().launch();
    }
  })();
  std::cout << t << std::endl;

  for (size_t i = 0; i < 500; i++) {
    std::cout << sum << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

namespace {

struct uniform_random {
  std::mt19937_64 gen{std::random_device{}()};
  std::uniform_int_distribution<int> dist;
  std::mutex mutex;

  uniform_random(int l, int r) : dist{l, r} {}
  int operator()() {
    std::lock_guard lock{mutex};
    return dist(gen);
  }
};

struct toy_server {
  struct request_t {
    int num_of_char;
  };

  enum class code_t : uint8_t { Done, Streaming };

  struct response_t {
    code_t code;
    std::string content;
  };

  using msg_t = msg::message<std::variant<std::monostate, response_t>>;
  using stream_t = sync_stream<msg_t>;

  uniform_random rd{1, 100};
  std::vector<guarded_thread> tasks;

  void request(request_t req, std::weak_ptr<stream_t> receive) {
    tasks.emplace_back(std::thread{[this, req, receive = std::move(receive)]() {
      for (size_t i = 0; i < req.num_of_char; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rd()));
        if (auto _receive = receive.lock()) {
          _receive->write_sync(response_t{.code = code_t::Streaming, .content = "x"});
        } else {
          break;
        }
      }
      if (auto _receive = receive.lock()) {
        _receive->write_sync(response_t{.code = code_t::Done});
      }
    }});
  }
};

struct toy_client {
  using request_t = toy_server::request_t;
  using response_t = toy_server::response_t;
  using msg_t = msg::message<std::variant<std::monostate, response_t>>;
  using stream_t = sync_stream<msg_t>;

  struct stream_response_t {
    std::shared_ptr<stream_t> stream;
    std::unique_ptr<runner<to_execute_t<msg_t, read_stream, stream_t>>> worker{
        new runner<to_execute_t<msg_t, read_stream, stream_t>>{3}};  // cap = 8
    using dispatcher_t = decltype(stream->get_dispatcher(read_stream{}, *worker));
    dispatcher_t dispatcher = stream->get_dispatcher(read_stream{}, *worker);

    using chunk_t = msg::message<std::variant<std::monostate, std::string_view>>;

    struct chunk_awaitable : public dispatcher_t::awaitable {
      using base = dispatcher_t::awaitable;

      chunk_t await_resume() {
        auto ret = base::await_resume();
        if (ret == stream_t::status::good) {
          if (data.data_t_same_as<response_t>()) {
            auto& resp = data.get<response_t>();
            if (resp.code == toy_server::code_t::Streaming) {
              return chunk_t{std::string_view{resp.content}};
            }
          }
        }
        return chunk_t{};
      }
    };

    auto get_chunk(msg_t& msg_buffer) {
      return chunk_awaitable{dispatcher(msg_buffer)};
    }
  };

  stream_response_t stream_request(toy_server& server, int num) {
    stream_response_t resp{std::make_shared<stream_t>()};
    server.request(request_t{.num_of_char = num}, resp.stream);
    return resp;
  }
};

// template <std::movable T, std::invocable<T> Stop, std::invocable<T> Fetch>
//   requires std::is_same_v<std::invoke_result_t<Stop, T>, bool>
struct toy_range {
  using T = msg::message<std::variant<std::monostate, std::string>>;
  using Stop = decltype([](const T& data) -> bool { return !(bool)data; });

  struct iterator {
    // T operator*() {}
  };
};

}  // namespace

void try_await2() {
  auto server = toy_server{};
  auto client = toy_client{};
  using msg_t = toy_client::msg_t;
  using response_t = toy_client::response_t;
  auto resp = client.stream_request(server, 100);
  using resp_t = decltype(resp);

  auto task = [](resp_t& resp) -> co_task {
    msg_t msg_buffer;
    while (true) {
      auto chunk = co_await resp.get_chunk(msg_buffer);
      if (chunk) {
        auto ch = std::get<std::string_view>(chunk.data);
        std::cout << ch << std::flush;
      } else {
        break;
      }
    }
    std::cout << std::endl;
  }(resp).launch();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  task.sync_wait();
}

void try_await3() {
  auto server = toy_server{};
  auto client = toy_client{};
  using msg_t = toy_client::msg_t;
  using response_t = toy_client::response_t;
  auto resp = client.stream_request(server, 100);
  using resp_t = decltype(resp);

  auto task = [](resp_t& resp) -> co_task {
    msg_t msg_buffer;
    while (true) {
      // no capture
      auto mini_task = [](resp_t& resp, msg_t& buffer) -> co_task_with<resp_t::chunk_t> {
        co_return co_await resp.get_chunk(buffer);
      }(resp, msg_buffer);

      auto chunk = co_await mini_task.wait();
      if (chunk) {
        auto ch = std::get<std::string_view>(chunk.data);
        std::cout << ch << std::flush;
      } else {
        break;
      }
    }
    std::cout << std::endl;
  }(resp).launch();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  task.sync_wait();
}

void try_await4() {
  auto server = toy_server{};
  auto client = toy_client{};
  using msg_t = toy_client::msg_t;
  using response_t = toy_client::response_t;
  auto resp = client.stream_request(server, 100);

  auto executor = runner<cancellable_function<void>>{};

  auto task = []<typename resp_t, typename executor_t>(resp_t& resp,
                                                       executor_t& executor) -> co_task {
    msg_t msg_buffer;
    while (true) {
      auto chunk = co_await resp.get_chunk(msg_buffer);
      co_await execute_by(executor);
      if (chunk) {
        auto ch = std::get<std::string_view>(chunk.data);
        std::cout << ch << std::flush;
      } else {
        break;
      }
    }
    std::cout << std::endl;
  }(resp, executor).launch();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  task.sync_wait();
}

void try_await5() {
  auto server = toy_server{};
  auto client = toy_client{};
  using msg_t = toy_client::msg_t;
  using response_t = toy_client::response_t;
  auto resp = client.stream_request(server, 100);

  auto executor = runner<cancellable_function<void>>{};

  auto task = []<typename resp_t, typename executor_t>(resp_t& resp,
                                                       executor_t& executor) -> co_task {
    msg_t msg_buffer;
    while (true) {
      auto chunk = co_await resp.get_chunk(msg_buffer);
      co_await execute_by(executor);
      if (chunk) {
        auto ch = std::get<std::string_view>(chunk.data);
        std::cout << ch << std::flush;
      } else {
        break;
      }
    }
    std::cout << std::endl;
  }(resp, executor).launch();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  // task.sync_wait();
  // 提前结束, 析构 executor 和 resp (RAII 管理 IO runner), 测试是否能正确 destroy 等待 resume
  // 的协程.
}

}  // namespace playground

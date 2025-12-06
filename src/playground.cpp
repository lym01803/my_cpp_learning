#include <playground.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <locale>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <random>
#include <ranges>
#include <ratio>
#include <shared_mutex>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "message.h"

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

void try_async_stream() {
  const auto N = 100'000'000LL;
  const size_t mod = 998244353;
  std::vector data{std::from_range, std::views::iota(1LL, N + 1LL)};

  using msg_t = msg::message<std::variant<std::monostate, sum_msg, square_sum_msg>>;
  async_stream<msg_t> as;
  counter_controller counter{.callback = [&]() { as.stop(); }};

  guarded_thread task1{std::thread{[&]() {
    counter_controller::guard guard{counter};
    for (int i = 0; i < 5; i++) {
      as.write_sync(sum_msg{std::ranges::fold_left(data, (size_t)0, std::plus<size_t>{})});
    }
  }}};

  guarded_thread task2{std::thread{[&]() {
    counter_controller::guard guard{counter};
    for (int i = 0; i < 5; i++) {
      as.write_sync(square_sum_msg{std::ranges::fold_left(data, (size_t)0, [](size_t s, size_t v) {
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
    while (as) {
      msg_t msg;
      if (as.read_sync(msg) == async_stream<msg_t>::status::good) {
        std::visit(output, msg.data);
      }
    }
  }}};
}

void try_coroutine() {}

}  // namespace playground

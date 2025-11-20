#include <playground.h>

#include <algorithm>
#include <chrono>
#include <concepts>
#include <exception>
#include <iostream>
#include <mutex>
#include <optional>
#include <ranges>
#include <ratio>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <utility>

namespace playground {

void try_concurrency() {
  auto f = [](std::string s) { std::cout << s << std::endl; };
  guarded_thread t{std::thread{f, "launch"}};

  std::vector<std::string> strings{"Hello", ",", " ", "world", "!", "\n"};
  std::vector<guarded_thread> threads;
  threads.reserve(strings.size());
  for (const auto& str : strings) {
    threads.emplace_back(std::thread{
        [](const std::string& s) { std::cout << s; }, std::ref(str)});
  }
  for (auto& t : threads) {
    if (t->joinable()) {
      t->join();
    }
  }
  std::cout << std::endl;

  std::cout << std::format("hardware_concurrency: {}",
                           std::thread::hardware_concurrency())
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

  guarded_thread calc_sum{
      std::thread{[](const decltype(nums)& vec, long long& result) {
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

  guarded_thread calc_sum{
      std::thread{[&mutex](const decltype(nums)& vec, long long& result) {
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
    std::cout << std::format("time: {}", timer_wrap(sum_and_add_without_lock)())
              << std::endl;
    std::cout << std::format("time: {}", timer_wrap(sum_and_add_with_lock)())
              << std::endl;
  }
}

}  // namespace playground

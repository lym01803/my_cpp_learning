#include <playground.h>

#include <algorithm>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <exception>
#include <iostream>
#include <mdspan>
#include <mutex>
#include <optional>
#include <ranges>
#include <ratio>
#include <span>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <utility>
#include <version>

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

void play_with_matrix() {
  Matrix2D<int> m{3, 4};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      m[i, j] = (i + 1) * (j + 1);
    }
  }
  Matrix2D<int> m2(m);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      m2[i, j] = -m2[i, j];
    }
  }
  std::cout << m << std::endl;
  std::cout << m2 << std::endl;
  Matrix2D<double> n(m);
  n[0, 0] = 3.14;
  std::cout << n << std::endl;
  Matrix2D<int> m3{3, 4};
  std::cout << m3 << std::endl;
  m3 = std::move(m);
  std::cout << m3 << std::endl;

  std::vector<int> data{std::from_range, std::views::iota(1, 12 + 1)};
  std::mdspan views{data.data(), 3, 4};
  views[0, 0] = 0;
  for (int i = 0; i < views.extent(0); i++) {
    for (int j = 0; j < views.extent(1); j++) {
      std::cout << views[i, j] << " ";
    }
    std::cout << "\n";
  }
  std::cout << std::endl;
}

void play_with_tick_timer() {
  tick_timer timer {};
  auto t = timer();
  for (int i = 0; i < 100; i++) {
    std::cout << timer() - t<< std::endl;
    std::this_thread::sleep_for(std::chrono::microseconds {300});
  }
}

}  // namespace playground

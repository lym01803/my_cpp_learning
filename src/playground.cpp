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
#include <shared_mutex>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
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
  std::vector arr{std::from_range, std::views::iota(1LL, 1000LL + 1LL)};
  auto get_sum = [&s_mutex](const auto& arr, size_t N) {
    typename std::remove_cvref_t<decltype(arr)>::value_type sum = 0;
    std::shared_lock<std::shared_mutex> lock{s_mutex};
    for (size_t i = 0; i < N; i++) {
      if (i % 100 == 0) {
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        lock.lock();
      }
      sum += arr[i];
    }
    return sum;
  };
  auto modify_arr = [&s_mutex](auto& arr, size_t N) {
    std::unique_lock<std::shared_mutex> lock{s_mutex};
    for (size_t i = 0; i < N; i++) {
      if (i % 100 == 0) {
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
        lock.lock();
      }
      arr[i] += 1;
    }
  };
  guarded_thread reader_1{std::thread{[&s_mutex, &arr, &get_sum]() {
    auto result = get_sum(arr, arr.size());
    std::cout << "reader 1 sum: " << result
              << std::endl;  // expect: 500500 + 100 * k
  }}};
  guarded_thread reader_2{std::thread{[&s_mutex, &arr, &get_sum]() {
    auto result = get_sum(arr, arr.size());
    std::cout << "reader 2 sum: " << result
              << std::endl;  // expect: 500500 + 100 * k
  }}};
  guarded_thread writer{std::thread{
      [&s_mutex, &arr, &modify_arr]() { modify_arr(arr, arr.size()); }}};
}

}  // namespace playground

#pragma once

#include <array>
#include <cstdlib>
#include <memory>
#include <type_traits>

namespace toyqueue {

template <typename T, size_t N>
  requires requires { std::array<T, N + 1>{}; } && std::is_move_assignable_v<T>
class circular_queue {
  static constexpr size_t capacity = N;
  using value_type = T;

  std::array<T, N + 1> queue{};
  size_t head{0};
  size_t tail{0};

  void shift_tail() noexcept {
    tail = next_index(tail);
    if (head == tail) {
      head = next_index(head);
    }
  }

  void shift_head() {
    head = next_index(head);
  }

 public:
  bool empty() const noexcept {
    return head == tail;
  }

  static size_t next_index(size_t index) noexcept {
    return index == N ? 0 : index + 1;
  }

  static size_t prev_index(size_t index) noexcept {
    return index == 0 ? N : index - 1;
  }

  T& push(T value) noexcept(std::is_nothrow_move_assignable_v<T>) {
    queue[tail] = std::move(value);
    T& ref = queue[tail];
    shift_tail();
    return ref;
  }

  void pop() noexcept(std::is_nothrow_move_assignable_v<T>) {
    std::destroy_at(&queue[head]);
    shift_head();
  }

  T const& front() const noexcept {
    return queue[head];
  }

  T& front() noexcept {
    return queue[head];
  }
};

};  // namespace toyqueue

#pragma once

#include <stdatomic.h>

#include <array>
#include <atomic>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <optional>
#include <thread>
#include <type_traits>
#include <vector>

namespace toyqueue {

enum class queue_location_status : uint8_t { empty, busy, not_empty };

template <std::movable T>
class fix_cap_queue {
 public:
  using value_t = T;
  using status = queue_location_status;

 private:
  struct location {
    std::optional<value_t> data;
    std::atomic<status> flag{status::empty};
  };
  using container_t = std::vector<location>;

  container_t array;
  std::atomic<size_t> head{0};
  std::atomic<size_t> tail{0};
  const size_t cap;

  struct flag_guard {
    std::atomic<status>& flag;
    status set_to{};
    ~flag_guard() {
      flag.store(set_to, std::memory_order_release);
    }
  };

  struct clear_data_guard {
    std::optional<value_t>& data;
    bool disable{false};
    ~clear_data_guard() {
      if (!disable) {
        data = std::nullopt;
      }
    }
  };

 public:
  fix_cap_queue(size_t cap) : array(cap + 1), cap{cap} {}

  /** @warning validity not guaranteed under concurrency;
               应在有锁的情形下使用, 或在生产者已停止时用作判定结束的谓词;
               若无 push 操作发生 (生产者已停止), empty() 返回 true, 则队列一定为空;
               返回 false 则队列仍可能因为并发的 pop 变为空;
   */
  bool empty() const noexcept {
    return head.load(std::memory_order_relaxed) == tail.load(std::memory_order_relaxed);
  }

  /**
   @return optional<value_t>, 尝试 pop 失败时返回 nullopt;
   @note 当且仅当 pop 成功时, 通过 CAS 原子地移动 head 指针;
         pop 失败当且仅当尝试 CAS 前看到的队列为空 (from the view of this thread);
         自旋地等到 data location 的 flag 为 not_empty, 才会 pop 并返回 front;
         当 data location 的 flag 置为 empty 时 (memory_order_release), pop 完成;
         应尽可能保证 T 类型 移动构造/赋值 nothrow, 接收返回值时发生异常会丢失该数据, 但队列仍有效;
   */
  std::optional<value_t> try_pop() noexcept(std::is_nothrow_move_constructible_v<T>) {
    size_t cur_head = head.load(std::memory_order_relaxed);
    bool not_empty{};
    // 如果 CAS 成功, 则判定 not_empty 时的 cur_head 一定是即时的值, 而 tail 不会回退,
    // 故 not_empty 在 CAS 成功时有效 (from the view of this thread)
    while ((not_empty = cur_head != tail.load(std::memory_order_relaxed)) &&
           !head.compare_exchange_weak(cur_head, next_index(cur_head), std::memory_order_relaxed)) {
      std::this_thread::yield();
    }
    if (!not_empty) {
      return std::nullopt;
    }
    location& loc = array[cur_head];
    // not_mpty -> busy -> empty
    status expected = status::not_empty;
    while (!loc.flag.compare_exchange_weak(expected, status::busy, std::memory_order_acquire,
                                           std::memory_order_relaxed)) {
      expected = status::not_empty;
      std::this_thread::yield();
    }
    // return move(loc.data) -> set loc.data to nullopt -> set flag to empty (release)
    flag_guard fg{.flag = loc.flag, .set_to = status::empty};
    clear_data_guard dg{loc.data};
    // 显式移交 loc.data
    return std::move(loc.data);
  }

  /**
   @return bool, 尝试 push 失败时返回 false;
   @note 当且仅当 push 成功时, 通过 CAS 原子地移动 tail 指针;
         push 失败当且仅当尝试 CAS 前看到的队列为满 (from the view of this thread);
         自旋地等到 data location 的 flag 为 empty, 才会 push 写入, 并返回 true;
         当 data location 的 flag 置为 not_empty 时 (memory_order_release), push 完成;
         应尽可能保证 T 类型 移动构造/赋值 nothrow, push 构造时发生异常会丢失数据, 但队列仍有效;
   */
  bool try_push(T value) noexcept(std::is_nothrow_move_constructible_v<T>) {
    size_t cur_tail = tail.load(std::memory_order_relaxed);
    size_t next_tail = next_index(cur_tail);
    bool not_full{};
    // 如果 CAS 成功, 则判定 not_full 时的 cur_tail 一定是即时的值, 而 head 不会回退,
    // 故 not_full 在 CAS 成功时有效 (from the view of this thread)
    while ((not_full = next_tail != head.load(std::memory_order_relaxed)) &&
           !tail.compare_exchange_weak(cur_tail, next_tail, std::memory_order_relaxed)) {
      next_tail = next_index(cur_tail);
      std::this_thread::yield();
    }
    if (!not_full) {
      return false;
    }
    location& loc = array[cur_tail];
    // empty -> busy -> not_empty
    status expected = status::empty;
    while (!loc.flag.compare_exchange_weak(expected, status::busy, std::memory_order_acquire,
                                           std::memory_order_relaxed)) {
      expected = status::empty;
      std::this_thread::yield();
    }
    // loc.data = std::move(value) -> set flag to not_empty (release)
    flag_guard fg{.flag = loc.flag, .set_to = status::not_empty};
    // when exception, set data to nullopt
    clear_data_guard dg{loc.data};
    loc.data = std::move(value);
    // not set data to nullopt
    dg.disable = true;
    return true;
  }

 private:
  size_t next_index(size_t index) const noexcept {
    return index == cap ? 0 : index + 1;
  }
};

};  // namespace toyqueue

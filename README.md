# C++ Playground: Ranges, Concurrency, Coroutine

这是一个 C++ 学习实验仓库。记录了从最初探索 C++20 Ranges 特性，逐步转向学习 C++ 并发，并从零实现一套 C++20 异步协程基础设施的过程。

## 核心库介绍 (`include/`)

### `async_tool.h`
本项目最核心的组件，一套从零实现的 C++20 协程异步框架。
*   **`co_task`**: 通用异步任务对象，支持 Lazy Execution。提供三种灵活的生命周期管理模式：
    *   `detach()`: 分离执行（Fire-and-forget）。
    *   `get_future()`: 桥接异步与同步，用于在非协程环境等待结果。
    *   `co_await`: 协程间的逻辑组合。
*   **`lift`**: 一个强大的包装器，用于将普通的函数、Awaitable 转换为具备“上下文感知”的任务。
    *   用法示例：`co_await lift(task).on(executor)` 将任务切到指定线程执行；使用 `.back_to(executor)` 在执行完后自动切回。
*   **`all`**: 实现类似 `Promise.all` 的功能，支持并行等待多个任务。
    *   支持变长参数：`co_await all(task1, task2, ...)`，返回包含结果的 `std::tuple`。
    *   支持 Range/Container：`co_await all(std::from_range, task_range)`，返回包含结果的 `std::vector`。
*   **对称转移 (Symmetric Transfer)**: 内部完全采用对称转移机制，确保了深度协程调用链下的栈安全性与高性能。

### `message.h`
基于 `std::variant` 实现的类型安全消息系统。
*   **`message<V>`**: 消息包装器，为 payload 自动关联序列号（Serial Number）和时间戳（Timestamp）。
*   **Compile-time Validation**: 利用 C++20 Concepts（如 `valid_msg_variant`）在编译期确保消息类型的唯一性与合法性（例如必须以 `std::monostate` 开头且无重复类型）。

### `toyqueue.h`
用于学习和对比的高性能并发队列。
*   **`fix_cap_queue`**: 一个高性能的无锁（Lock-free）固定容量 MPMC 队列。使用 `std::atomic` 和细粒度的状态管理（flag-based）来避免 ABA 问题，适用于极高并发的任务调度。
*   **`naive_fix_cap_queue`**: 基于传统环形缓冲区的实现，常用于与无锁版本进行性能基准测试。

## 运行时工具 (`include/playground.h`)

*   **`runner`**: 调度器/执行器实现。它封装了一个 `fix_cap_queue` 任务队列和一条专用线程，负责驱动协程状态机的 Resume 动作，是 `async_tool` 运行的引擎。
*   **`sync_stream`**: 同步消息流。它结合了队列与 `stoppable_cv`（可停止的条件变量），提供阻塞式的 `read_sync` 和 `write_sync` 接口，是 Demo 中线程间通信的主要通道。

## 实验演示 (`src/playground.cpp`)

该文件记录了所有特性的演进过程与 Demo 实现：

*   **Ranges 实验**: 早期对 `std::ranges` 管道操作（如 `iota`, `transform`, `join`）的探索。
*   **传统并发**: 涉及 `std::mutex`, `std::shared_mutex`, `std::condition_variable` 以及原子操作的并发模型测试。
*   **协程演进 (try_await 系列)**: 从 `try_await` 到 `try_await13` 记录了协程库从零开始的每一个里程碑：
    *   **基础模式**: 简单的协程挂起与恢复。
    *   **生成器**: 使用 `co_yield` 实现异步生成器。
    *   **复杂集成**: 最终 Demo (`try_await13`) 展示了一个完整的生产者-消费者应用：后台线程计算质数，并通过协程自动切换上下文，在模拟的 GUI 线程上实时更新进度条。
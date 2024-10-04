///
/// \file      thread_safe_queue.hpp
/// \author    https://www.geeksforgeeks.org/implement-thread-safe-queue-in-c/
/// \date      2024-10-04
///

// C++ implementation of the above approach
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

namespace joda {

// Thread-safe queue
template <typename T>
class TSQueue
{
private:
  // Underlying queue
  std::queue<T> m_queue;

  // mutex for thread synchronization
  std::mutex m_mutex;

  // Condition variable for signaling
  std::condition_variable m_cond;

  bool mStopped = false;

public:
  // Pushes an element to the queue
  void push(T item)
  {
    // Acquire lock
    std::unique_lock<std::mutex> lock(m_mutex);

    // Add item
    m_queue.push(item);

    // Notify one thread that
    // is waiting
    m_cond.notify_one();
  }

  // Pops an element off the queue
  T pop()
  {
    // acquire lock
    std::unique_lock<std::mutex> lock(m_mutex);

    // wait until queue is not empty
    m_cond.wait(lock, [this]() { return (!m_queue.empty() || mStopped); });

    // retrieve item
    T item = m_queue.front();
    m_queue.pop();

    // return item
    return item;
  }

  [[nodiscard]] bool isEmpty() const
  {
    return m_queue.empty();
  }

  void stop()
  {
    mStopped = true;
  }
};
}    // namespace joda

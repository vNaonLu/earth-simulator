#ifndef __ESIM_CORE_CORE_FIFO_H_
#define __ESIM_CORE_CORE_FIFO_H_

#include "utils.h"
#include <atomic>
#include <cassert>
#include <thread>
#include <vector>

namespace esim {

namespace core {

namespace details {

template <typename type>
class alignas(128UL) fifo_item {
public:
  constexpr static std::allocator<type>          alloc;
  typedef std::allocator_traits<decltype(alloc)> alloctraits;

  bool active(std::memory_order order = std::memory_order_acquire) const noexcept;

  template <typename... args_type>
  void emplace(args_type&&... args);

  type get() noexcept;

  fifo_item() noexcept;

  ~fifo_item() noexcept;

private:
  rptr<type>        data_;
  std::atomic<bool> active_;
};

} // namespace details

/**
 * @brief Thread-safety circular First-In-First-Out queue
 * 
 * @tparam type specifies the target type of content.
 * @note multiple producer multiple consumer supported.
 */
template <typename type>
class fifo {
public:
  /**
   * @brief Pop the first item from the queue
   * 
   * @tparam l_type specifies the return type, must be convertible from 'type'.
   * @param ref specifies the reference to receive.
   */
  template <typename l_type>
  void pop(l_type &ref) noexcept;

  /**
   * @brief Try to pop the first item from the queue
   * 
   * @tparam l_type specifies the return type, must be convertible from 'type'.
   * @param ref specifies the reference to receive.
   * @return true if operates successful, false otherwise.
   */
  template <typename l_type>
  bool try_pop(l_type &ref) noexcept;

  /**
   * @brief Push the item into the queue
   * 
   * @tparam r_type specifies the type of target item. must be convertible to 'type'.
   * @param ref specifies the target item.
   */
  template <typename r_type>
  void push(r_type &&ref) noexcept;

  /**
   * @brief Try to push the item into the queue
   * 
   * @tparam r_type specifies the type of target item. must be convertible to 'type'.
   * @param ref specifies the target item.
   * @return true if operates successful, false otherwise.
   */
  template <typename r_type>
  bool try_push(r_type &&ref) noexcept;

  /**
   * @brief Construct the item directly into the queue
   * 
   * @param ref specifies the arguments of consturctor.
   */
  template <typename... types>
  void emplace(types &&...ref) noexcept;

  /**
   * @brief Try to construct the item directly into the queue
   * 
   * @param ref specifies the arguments of consturctor.
   * @return true if operates successful, false otherwise.
   */
  template <typename... types>
  bool try_emplace(types &&...ref) noexcept;

  /**
   * @brief Check if the queue is empty
   * 
   * @return true if fifo is empty, false otherwise.
   */
   bool empty() const noexcept;

  /**
   * @brief Construct a new fifo object
   * 
   * @param size specifies the size of fifo.
   */
  explicit fifo(uint32_t size = 128) noexcept;

private:
  uint32_t head(std::memory_order order = std::memory_order_acquire) const noexcept;
  uint32_t tail(std::memory_order order = std::memory_order_acquire) const noexcept;
  uint32_t fetch_head(std::memory_order order = std::memory_order_release) noexcept;
  uint32_t fetch_tail(std::memory_order order = std::memory_order_release) noexcept;
  bool cas_head(uint32_t &ref, std::memory_order order = std::memory_order_release) noexcept;
  bool cas_tail(uint32_t &ref, std::memory_order order = std::memory_order_release) noexcept;

private:
  const uint32_t size_;
  const uint32_t mask_;
  alignas(128UL) std::atomic<uint32_t>  head_,
                                        tail_;
  std::vector<details::fifo_item<type>> data_;
};

} // namespace core

} // namespace esim

#include "fifo.inl"

#endif
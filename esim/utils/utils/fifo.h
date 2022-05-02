#ifndef __ESIM_UTILS_UTILS_FIFO_H_
#define __ESIM_UTILS_UTILS_FIFO_H_

#include "numeric.h"
#include "utils.h"
#include <atomic>
#include <cassert>
#include <thread>
#include <vector>

namespace esim {

namespace utils {

namespace details {

template <typename type>
class alignas(128UL) fifo_item {
public:
  constexpr static std::allocator<type>          alloc;
  typedef std::allocator_traits<decltype(alloc)> alloctraits;

  inline bool active(std::memory_order order = std::memory_order_acquire) const noexcept {

    return active_.load(order);
  }

  template <typename... args_type>
  inline void emplace(args_type&&... args) {
    assert(!active());
    alloctraits::construct(alloc, data_, std::forward<args_type>(args)...);
    active_.store(true, std::memory_order_release);
  }

  inline type get() noexcept {
    assert(active());
    active_.store(false, std::memory_order_release);

    return std::move(*data_);
  }

  fifo_item() noexcept
      : data_{nullptr}, active_{false} {
    data_ = alloctraits::allocate(alloc, 1);
  }

  ~fifo_item() noexcept {
    if (active()) {
      get(); /// destory by return reference
    }
    alloctraits::deallocate(alloc, data_, 1);
  }

private:
  r_ptr<type>       data_;
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
  inline void pop(l_type &ref) noexcept {
    static_assert(std::is_convertible_v<type, l_type>);
    auto index = fetch_tail();
    while (!data_[index & mask_].active()) {
      std::this_thread::yield();
    }

    ref = data_[index & mask_].get();
  }
  
  /**
   * @brief Try to pop the first item from the queue
   * 
   * @tparam l_type specifies the return type, must be convertible from 'type'.
   * @param ref specifies the reference to receive.
   * @return true if operates successful, false otherwise.
   */
  template <typename l_type>
  inline bool try_pop(l_type &ref) noexcept {
    static_assert(std::is_convertible_v<type, l_type>);
    auto index = tail();
    if (data_[index & mask_].active() && cas_tail(index)) {
      ref = data_[index & mask_].get();

      return true;
    }

    return false;
  }

  /**
   * @brief Push the item into the queue
   * 
   * @tparam r_type specifies the type of target item. must be convertible to 'type'.
   * @param ref specifies the target item.
   */
  template <typename r_type>
  inline void push(r_type &&ref) noexcept {
    static_assert(std::is_convertible_v<r_type, type>);
    auto index = fetch_head();
    while (data_[index & mask_].active()) {
      std::this_thread::yield();
    }

    data_[index & mask_].emplace(std::forward<r_type>(ref));
  }
  
  /**
   * @brief Try to push the item into the queue
   * 
   * @tparam r_type specifies the type of target item. must be convertible to 'type'.
   * @param ref specifies the target item.
   * @return true if operates successful, false otherwise.
   */
  template <typename r_type>
  inline bool try_push(r_type &&ref) noexcept {
    static_assert(std::is_convertible_v<r_type, type>);
    auto index = head();
    if (!data_[index & mask_].active() && cas_head(index)) {
      data_[index & mask_].emplace(std::forward<r_type>(ref));

      return true;
    }

    return false;
  }

  /**
   * @brief Construct the item directly into the queue
   * 
   * @param ref specifies the arguments of consturctor.
   */
  template <typename... types>
  inline void emplace(types &&...ref) noexcept {
    static_assert(std::is_constructible_v<type, types...>);
    auto index = fetch_head();
    while (data_[index & mask_].active()) {
      std::this_thread::yield();
    }

    data_[index & mask_].emplace(std::forward<types>(ref)...);
  }
  
  /**
   * @brief Try to construct the item directly into the queue
   * 
   * @param ref specifies the arguments of consturctor.
   * @return true if operates successful, false otherwise.
   */
  template <typename... types>
  inline bool try_emplace(types &&...ref) noexcept {
    static_assert(std::is_constructible_v<type, types...>);
    static_assert(std::is_convertible_v<r_type, type>);
    auto index = head();
    if (!data_[index & mask_].active() && cas_head(index)) {
      data_[index & mask_].emplace(std::forward<types>(ref)...);

      return true;
    }

    return false;
  }

  /**
   * @brief Check if the queue is empty
   * 
   * @return true if fifo is empty, false otherwise.
   */
  inline bool empty() const noexcept {

    return head(std::memory_order_relaxed) == tail(memory_order_relaxed);
  }

  /**
   * @brief Construct a new fifo object
   * 
   * @param size specifies the size of fifo.
   */
  explicit fifo(uint32_t size = 128) noexcept
      : size_{ceil2_32(size)},
        mask_{size_ - 1},
        head_{0}, tail_{0},
        data_(size_) {}

private:
  inline uint32_t head(std::memory_order order = std::memory_order_acquire) const noexcept {
    
    return head_.load(order);
  }

  inline uint32_t tail(std::memory_order order = std::memory_order_acquire) const noexcept {

    return tail_.load(order);
  }

  inline uint32_t fetch_head(std::memory_order order = std::memory_order_release) noexcept {

    return head_.fetch_add(1, order);
  }
  
  inline uint32_t fetch_tail(std::memory_order order = std::memory_order_release) noexcept {
    
    return tail_.fetch_add(1, order);
  }

  inline bool cas_head(uint32_t &ref,
                       std::memory_order order = std::memory_order_release) noexcept {

    return head_.compare_exchange_strong(ref, ref + 1, order);
  }

  inline bool cas_tail(uint32_t &ref,
                       std::memory_order order = std::memory_order_release) noexcept {

    return tail_.compare_exchange_strong(ref, ref + 1, order, std::memory_order_relaxed);
  }

private:
  const uint32_t size_;
  const uint32_t mask_;
  alignas(128UL) std::atomic<uint32_t>  head_,
                                        tail_;
  std::vector<details::fifo_item<type>> data_;
};

} // namespace utils

} // namespace esim

#endif 
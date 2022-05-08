namespace esim {

namespace core {

namespace details {

template <typename type>
inline bool fifo_item<type>::active(std::memory_order order) const noexcept {

  return active_.load(order);
}

template <typename type>
template <typename... args_type>
inline void fifo_item<type>::emplace(args_type &&...args) {
  assert(!active());
  alloctraits::construct(alloc, data_, std::forward<args_type>(args)...);
  active_.store(true, std::memory_order_release);
}

template <typename type>
inline type fifo_item<type>::get() noexcept {
  assert(active());
  active_.store(false, std::memory_order_release);

  return std::move(*data_);
}

template <typename type>
inline fifo_item<type>::fifo_item() noexcept
    : data_{nullptr}, active_{false} {
  data_ = alloctraits::allocate(alloc, 1);
}

template <typename type>
inline fifo_item<type>::~fifo_item() noexcept {
  if (active()) {
    get(); /// destory by return reference
  }
  alloctraits::deallocate(alloc, data_, 1);
}

} // namespace details

template <typename type>
template <typename l_type>
inline void fifo<type>::pop(l_type &ref) noexcept {
  static_assert(std::is_convertible_v<type, l_type>);
  auto index = fetch_tail();
  while (!data_[index & mask_].active()) {
    std::this_thread::yield();
  }

  ref = data_[index & mask_].get();
}

template <typename type>
template <typename l_type>
inline bool fifo<type>::try_pop(l_type &ref) noexcept {
  static_assert(std::is_convertible_v<type, l_type>);
  auto index = tail();
  if (data_[index & mask_].active() && cas_tail(index)) {
    ref = data_[index & mask_].get();

    return true;
  }

  return false;
}

template <typename type>
template <typename r_type>
inline void fifo<type>::push(r_type &&ref) noexcept {
  static_assert(std::is_convertible_v<r_type, type>);
  auto index = fetch_head();
  while (data_[index & mask_].active()) {
    std::this_thread::yield();
  }

  data_[index & mask_].emplace(std::forward<r_type>(ref));
}

template <typename type>
template <typename r_type>
inline bool fifo<type>::try_push(r_type &&ref) noexcept {
  static_assert(std::is_convertible_v<r_type, type>);
  auto index = head();
  if (!data_[index & mask_].active() && cas_head(index)) {
    data_[index & mask_].emplace(std::forward<r_type>(ref));

    return true;
  }

  return false;
}

template <typename type>
template <typename... types>
inline void fifo<type>::emplace(types &&...ref) noexcept {
  static_assert(std::is_constructible_v<type, types...>);
  auto index = fetch_head();
  while (data_[index & mask_].active()) {
    std::this_thread::yield();
  }

  data_[index & mask_].emplace(std::forward<types>(ref)...);
}

template <typename type>
template <typename... types>
inline bool fifo<type>::try_emplace(types &&...ref) noexcept {
  static_assert(std::is_constructible_v<type, types...>);
  auto index = head();
  if (!data_[index & mask_].active() && cas_head(index)) {
    data_[index & mask_].emplace(std::forward<types>(ref)...);

    return true;
  }

  return false;
}

template <typename type>
inline bool fifo<type>::empty() const noexcept {

  return head(std::memory_order_relaxed) == tail(std::memory_order_relaxed);
}

template <typename type>
inline fifo<type>::fifo(uint32_t size) noexcept
    : size_{ceil2_32(size)},
      mask_{size_ - 1},
      head_{0}, tail_{0},
      data_(size_) {}

template <typename type>
inline uint32_t fifo<type>::head(std::memory_order order) const noexcept {

  return head_.load(order);
}

template <typename type>
inline uint32_t fifo<type>::tail(std::memory_order order) const noexcept {

  return tail_.load(order);
}

template <typename type>
inline uint32_t fifo<type>::fetch_head(std::memory_order order) noexcept {

  return head_.fetch_add(1, order);
}

template <typename type>
inline uint32_t fifo<type>::fetch_tail(std::memory_order order) noexcept {

  return tail_.fetch_add(1, order);
}

template <typename type>
inline bool fifo<type>::cas_head(uint32_t &ref, std::memory_order order) noexcept {

  return head_.compare_exchange_strong(ref, ref + 1, order);
}

template <typename type>
inline bool fifo<type>::cas_tail(uint32_t &ref, std::memory_order order) noexcept {

  return tail_.compare_exchange_strong(ref, ref + 1, order, std::memory_order_relaxed);
}

} // namespace core

} // namespace esim

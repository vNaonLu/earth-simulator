#ifndef __ESIM_UTILS_UTILS_OBSERVER_H_
#define __ESIM_UTILS_UTILS_OBSERVER_H_

#include "utils.h"
#include <thread>
#include <unordered_set>
#include <iostream>

namespace esim {

namespace utils {

template <typename>
class observer_attorney;

namespace details {

/**
 * @brief Interface of observer structure.
 * 
 */
class iobserver {
public:
  /**
   * @brief Subscribe the specified observer object to wait their update.
   * 
   */
  inline void subscribe(r_ptr<iobserver> obs) noexcept {
    obs->register_observer(this);
    follow_.insert(obs);
  }

  /**
   * @brief Unsubscribe the specified observer object and no notification received from it.
   * 
   */
  inline void unsubscribe(r_ptr<iobserver> obs) noexcept {
    obs->unregister_observer(this);
    follow_.erase(obs);
  }
  
  /**
   * @brief Destroy the iobserver object.
   * 
   */
  virtual ~iobserver() noexcept {
    for (auto &obs : follow_) {
      obs->unregister_observer(this);
    }

    for (auto &obs : followers_) {
      obs->unsubscribe(this);
    }
  }

protected:
  /**
   * @brief Add the specified observer object to its subscriber and going to notify them.
   * 
   */
  inline void register_observer(r_ptr<iobserver> obs) noexcept {
    followers_.insert(obs);
  }

  /**
   * @brief Remove the specified observer object and will not notify them.
   * 
   */
  inline void unregister_observer(r_ptr<iobserver> obs) noexcept {
    followers_.erase(obs);
  }

  /**
   * @brief Receive the message from its observer.
   * 
   */
  virtual void update(r_ptr<void>) noexcept {}

  /**
   * @brief Notify its observers.
   * 
   */
  inline void notify(r_ptr<void> raw) noexcept {
    for (auto &obs : followers_) {
      obs->update(raw);
    }
  }
  
private:
  std::unordered_set<r_ptr<iobserver>> follow_;
  std::unordered_set<r_ptr<iobserver>> followers_;
};

} // namespace details

/**
 * @brief CRTP Observer object.
 * 
 * @tparam derived specified the customer dervied class
 */
template <typename derived>
class observer : public details::iobserver {
public:
  inline void update(r_ptr<void> raw) noexcept final {
    observer_attorney<derived>::on_receive(*static_cast<r_ptr<derived>>(this), raw);
  }
};

/**
 * @brief Observer attorney manages the access domain
 * 
 * @tparam type specified the CRTP derived class.
 * @note you need to implements the following function:
 *    1. void on_receive (void*) noexcept
 * and makes a friend class to observer_attorney.
 */
template<typename type>
class observer_attorney {
private:
  inline static void on_receive(type &obj, r_ptr<void> msg) noexcept {
    obj.on_receive(msg);
  }
  
  friend class observer<type>;
};


} // namespace utils

} // namespace esim

#endif
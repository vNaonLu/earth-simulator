#ifndef __ESIM_UTILS_UTILS_OBSERVER_H_
#define __ESIM_UTILS_UTILS_OBSERVER_H_

#include "utils.h"
#include <unordered_set>

namespace esim {

namespace utils {
/**
 * @brief Observer class manages the subscribe and notify interface
 * TODO: multiobserve
 */
template <typename message_type>
class observer {
public:
  /**
   * @brief Register other observer class to receive their message
   * 
   * @param ob specifies the target object.
   */
  inline void subscribe(r_ptr<observer> ob) noexcept {
    ob->subscriber_ = this;
  }

  /**
   * @brief Unregister other observer class
   */
  inline void unsubscribe(r_ptr<observer> ob) noexcept {
    ob->subscriber_ = nullptr;
  }

  /**
   * @brief Construct a new observer object
   * 
   */
  observer() noexcept : subscriber_{nullptr} {}

  /**
   * @brief Destroy the observer object
   * 
   */
  ~observer() noexcept {
    if (nullptr != subscriber_) {
      subscriber_->unsubscribe(this);
    }
  }

protected:
  inline void notify(message_type &&msg) noexcept {
    if (nullptr != subscriber_) {
      subscriber_->update(std::move(msg));
    }
  }
  
  virtual void update(message_type &&msg) noexcept = 0;

private:
  r_ptr<observer> subscriber_;
};

} // namespace utils

} // namespace esim

#endif
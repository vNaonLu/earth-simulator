#ifndef __ESIM_CORE_CORE_OBSERVER_H_
#define __ESIM_CORE_CORE_OBSERVER_H_

#include "subject.h"
#include "utils.h"
#include <unordered_set>

namespace esim {

namespace core {

class subject;

/**
 * @brief A observer which can subscribe the subject item.
 * 
 */
class observer {
public:
  /**
   * @brief Subscribe a subject class, and wait to receive the message
   * from the subject.
   *
   * @param subject specifies the target subject.
   */
  void subscribe(rptr<class subject> subject) noexcept;

  /**
   * @brief Unsubscribe the subject.
   * 
   * @param subject specifies the target subject.
   */
  void unsubscribe(rptr<class subject> subject) noexcept;

  /**
   * @brief Construct a new observer object.
   * 
   */
  observer() noexcept;

  /**
   * @brief Destroy the observer object.
   * 
   */
  virtual ~observer() noexcept;

private:
  friend class subject;
  virtual void update(rptr<void> msg) noexcept = 0;

private:
  std::unordered_set<rptr<subject>> followed_;
};

} // namespace details

} // namespace esim

#endif
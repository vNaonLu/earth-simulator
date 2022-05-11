#ifndef __ESIM_CORE_CORE_OBSERVER_H_
#define __ESIM_CORE_CORE_OBSERVER_H_

#include "publisher.h"
#include "utils.h"
#include <unordered_set>

namespace esim {

namespace core {

class publisher;

/**
 * @brief A observer which can subscribe the publisher item.
 * 
 */
class observer {
public:
  /**
   * @brief Subscribe a publisher class, and wait to receive the message
   * from the publisher.
   *
   * @param publisher specifies the target publisher.
   */
  void subscribe(rptr<class publisher> publisher) noexcept;

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
  friend class publisher;
  virtual void update(rptr<void> msg) noexcept = 0;

private:
  std::unordered_set<rptr<publisher>> followed_;
};

} // namespace details

} // namespace esim

#endif
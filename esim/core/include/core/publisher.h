#ifndef __ESIM_CORE_CORE_SUBJECT_H_
#define __ESIM_CORE_CORE_SUBJECT_H_

#include "observer.h"
#include "utils.h"
#include <unordered_set>

namespace esim {

namespace core {

class observer;

/**
 * @brief A Subject class which can be subscribed to
 * notify the observer class.
 *
 */
class publisher {
public:
  /**
   * @brief Construct a new publisher object.
   * 
   */
  publisher() noexcept;

  /**
   * @brief Destroy the publisher object
   * 
   */
  virtual ~publisher() noexcept;

protected:
  /**
   * @brief Notify the observers with message.
   *
   * @param msg specifies the target message.
   */
  void notify(rptr<void> msg) noexcept;

private:
  friend class observer;
  void _register(rptr<class observer> follower) noexcept;

  void _unregister(rptr<class observer> follower) noexcept;

private:
  std::unordered_set<rptr<observer>> followers_;

};

} // namespace details

} // namespace esim

#endif
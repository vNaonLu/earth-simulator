#include "core/observer.h"
#include "core/subject.h"

namespace esim {

namespace core {

void observer::subscribe(rptr<class subject> subject) noexcept {
  subject->_register(this);
  followed_.insert(subject);
}

void observer::unsubscribe(rptr<class subject> subject) noexcept {
  subject->_unregister(this);
  followed_.erase(subject);
}

observer::observer() noexcept {
}

observer::~observer() noexcept {
  for (auto &subject : followed_) {
    subject->_unregister(this);
  }
}

} // namespace details

} // namespace esim

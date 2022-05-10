#include "core/observer.h"
#include "core/publisher.h"

namespace esim {

namespace core {

void observer::subscribe(rptr<class publisher> publisher) noexcept {
  publisher->_register(this);
  followed_.insert(publisher);
}

void observer::unsubscribe(rptr<class publisher> publisher) noexcept {
  publisher->_unregister(this);
  followed_.erase(publisher);
}

observer::observer() noexcept {
}

observer::~observer() noexcept {
  for (auto &publisher : followed_) {
    publisher->_unregister(this);
  }
}

} // namespace details

} // namespace esim

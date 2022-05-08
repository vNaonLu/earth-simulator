#include "core/observer.h"
#include "core/subject.h"

namespace esim {

namespace core {

void subject::notify(rptr<void> msg) noexcept {
  for (auto &observer : followers_) {
    observer->update(msg);
  }
}

void subject::_register(rptr<class observer> follower) noexcept {
  followers_.insert(follower);
}

void subject::_unregister(rptr<class observer> follower) noexcept {
  followers_.erase(follower);
}

subject::subject() noexcept {
}

subject::~subject() noexcept {
  for (auto &observer : followers_) {
    observer->unsubscribe(this);
  }
}

} // namespace details

} // namespace esim

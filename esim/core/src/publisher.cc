#include "core/observer.h"
#include "core/publisher.h"

namespace esim {

namespace core {

void publisher::notify(rptr<void> msg) noexcept {
  for (auto &observer : followers_) {
    observer->update(msg);
  }
}

void publisher::_register(rptr<class observer> follower) noexcept {
  followers_.insert(follower);
}

void publisher::_unregister(rptr<class observer> follower) noexcept {
  followers_.erase(follower);
}

publisher::publisher() noexcept {
}

publisher::~publisher() noexcept {
  for(auto &observer : followers_) {
    observer->followed_.erase(this);
  }
  followers_.clear();
}

} // namespace details

} // namespace esim

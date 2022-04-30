#ifndef __ESIM_CONTROLLER_MOUSE_H_
#define __ESIM_CONTROLLER_MOUSE_H_

#include <camera.h>
#include <coord/transform.h>

namespace esim {

namespace ctrl {

namespace mouse {

inline static void vertical_zoom_in(camera &cmr, size_t tick) noexcept {
  auto pos = cmr.lla();
  auto up = cmr.up();

  pos.z -= pos.z * tick / 10;
  cmr.set_camera(pos, up);
}

inline static void vertical_zoom_out(camera &cmr, size_t tick) noexcept {
  auto pos = cmr.lla();
  auto up = cmr.up();

  pos.z += pos.z * tick / 10;
  cmr.set_camera(pos, up);
}

} // namespace mouse

} // namespace ctrl

} // namespace esim

#endif
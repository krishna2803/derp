//===-- Implementation of camera class ------------------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#include "camera.hpp"

namespace derp {

camera::camera(const glm::vec3 &_position, const glm::vec3 &_world_up,
               float _yaw, float _pitch)
    : position(_position), world_up(_world_up), yaw(_yaw), pitch(_pitch),
      speed(SPEED), sensitivity(SENSITIVITY), fov(FOV) {
  update_vectors();
}

auto camera::get_view_matrix() const -> glm::mat4 {
  return glm::lookAt(position, position + front, up);
}

auto camera::get_fov() const noexcept -> float { return fov; }

auto camera::keyboard_move(const direction dir, const float delta_time) noexcept
    -> void {
  const float velocity = speed * delta_time;
  switch (dir) {
  case direction::FORWARD:
    position += front * velocity;
    break;
  case direction::BACKWARD:
    position -= front * velocity;
    break;
  case direction::RIGHT:
    position += right * velocity;
    break;
  case direction::LEFT:
    position -= right * velocity;
    break;
  case direction::UP:
    position += up * velocity;
    break;
  case direction::DOWN:
    position -= up * velocity;
    break;
  default:
    break;
  }
  update_vectors();
}

auto camera::mouse_move(const float x_off, const float y_off,
                        const bool constrain_pitch) -> void {

  yaw += sensitivity * x_off;
  pitch += sensitivity * y_off;

  if (constrain_pitch) {
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
  }
  update_vectors();
}
auto camera::gamepad_move(const float left_axis_x, const float left_axis_y,
                          const float right_axis_x, const float right_axis_y,
                          const float delta_time, const bool constrain_pitch)
    -> void {
  const float velocity = speed * delta_time;
  position -= front * velocity * left_axis_y * delta_time * 100.0f;
  position += right * velocity * left_axis_x * delta_time * 100.0f;

  yaw += sensitivity * right_axis_x * 10.0f;
  pitch -= sensitivity * right_axis_y * 10.0f;

  if (constrain_pitch) {
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
  }

  update_vectors();
}

auto camera::mouse_scroll(const float offset) -> void {
  fov = glm::clamp(fov - offset, 1.0f, 90.0f);
}

auto camera::update_vectors() -> void {
  const float yaw_rad = glm::radians(yaw);
  const float pitch_rad = glm::radians(pitch);
  const float cos_pitch = std::cos(pitch_rad);

  const glm::vec3 new_front{std::cos(yaw_rad) * cos_pitch, std::sin(pitch_rad),
                            std::sin(yaw_rad) * cos_pitch};
  front = glm::normalize(new_front);
  right = glm::normalize(glm::cross(front, world_up));
  up = glm::normalize(glm::cross(right, front));
}

} // namespace derp

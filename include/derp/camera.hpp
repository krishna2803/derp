//===-- Implementation header for camera class ----------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include <utility>

namespace derp {

class camera {
private:
  static constexpr float YAW = -90.0f;
  static constexpr float PITCH = 0.0f;
  static constexpr float SPEED = 2.5f;
  static constexpr float SENSITIVITY = 0.1f;
  static constexpr float FOV = 45.0f;

  glm::vec3 position{};
  glm::vec3 front{};
  glm::vec3 up{};
  glm::vec3 right{};
  glm::vec3 world_up{};

  float yaw;
  float pitch;

  float speed;
  float sensitivity;
  float fov;

public:
  enum class direction { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

  explicit camera(const glm::vec3 &_position = glm::vec3(0.0f),
                  const glm::vec3 &_world_up = glm::vec3(0.0f, 1.0f, 0.0f),
                  float _yaw = YAW, float _pitch = PITCH);

  [[nodiscard]]
  auto get_view_matrix() const -> glm::mat4;

  [[nodiscard]]
  auto get_fov() const noexcept -> float;

  auto keyboard_move(direction dir, float delta_time = 1.0f / 60.0f) noexcept
      -> void;

  auto mouse_move(float x_off, float y_off, bool constrain_pitch = true)
      -> void;

  auto mouse_scroll(float offset) -> void;

private:
  auto update_vectors() -> void;
}; // class camera
} // namespace derp

//===-- Main executable for derp ------------------------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#include "derp/camera.hpp"
#include "derp/mesh.hpp"
#include "derp/texture.hpp"

#include <derp/shader.hpp>

#include <iostream>
#include <print>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

void fb_resize_callback(GLFWwindow *window, const int width, const int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double x, double y);
void scroll_callback(GLFWwindow *window, double x_off, double y_off);
void process_input(GLFWwindow *window);

struct CameraSystem {
  mutable derp::camera camera;
  mutable float last_x = WIDTH / 2.0f;
  mutable float last_y = HEIGHT / 2.0f;
  mutable bool first_mouse = true;
  mutable float delta_time = 0.0f;
  mutable float last_frame = 0.0f;
  mutable bool gamepad_present;
  mutable GLFWgamepadstate gamepad;
};

int main() {

  std::println("[INFO] Starting...");
  if (!glfwInit()) {
    std::println("[ERROR] Couldn't initialize GLFW.");
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  // glfwWindowHint(GLFW_DECORATED, false);
  glfwWindowHint(GLFW_RESIZABLE, false);

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "derp", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    std::println("[ERROR] Couldn't create GLFW window.");
    return -1;
  }

  glfwMakeContextCurrent(window);

  CameraSystem cs{derp::camera(glm::vec3(0.0f, 0.0f, 3.0f))};

  glfwSetWindowUserPointer(window, &cs);

  glfwSetFramebufferSizeCallback(window, fb_resize_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::println("[ERROR] Couldn't initialize GLAD.");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  // If your display is scaled, the framebuffer size is different from the
  // window size.
  {
    int fbSizeX, fbSizeY;
    glfwGetFramebufferSize(window, &fbSizeX, &fbSizeY);
    glViewport(0, 0, fbSizeX, fbSizeY);
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
    const char *name = glfwGetGamepadName(GLFW_JOYSTICK_1);
    std::println("[INFO] game pad {} connected", name);
    cs.gamepad_present = true;
  }

  glEnable(GL_DEPTH_TEST);

  {
    const auto *vertex_data =
        reinterpret_cast<const derp::mesh::vertex *>(derp::cube_vertices);
    std::vector cube_vert(vertex_data, vertex_data + 24);

    std::vector cube_ind(std::begin(derp::cube_indices),
                         std::end(derp::cube_indices));

    derp::mesh m(std::move(cube_vert), std::move(cube_ind));

    derp::shader s(RESOURCES_PATH "/shaders/normal.vert",
                   RESOURCES_PATH "/shaders/texture.frag");
    s.use();

    auto model = glm::identity<glm::mat4>();

    const auto projection =
        glm::perspective(glm::radians(cs.camera.get_fov()),
                         static_cast<float>(WIDTH) / HEIGHT, 0.1f, 500.0f);
    s["u_projection"] = projection;

    derp::texture t(RESOURCES_PATH "/textures/container.png");
    t.use();

    auto m_test =
        derp::mesh::from_obj(RESOURCES_PATH "/models/mario/mario.obj");

    m.use();

    while (!glfwWindowShouldClose(window)) {
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      const auto current_frame = static_cast<float>(glfwGetTime());
      cs.delta_time = current_frame - cs.last_frame;
      cs.last_frame = current_frame;

      process_input(window);

      s["u_model"] = model;
      s["u_view"] = cs.camera.get_view_matrix();

      m.draw();

      m_test.use_and_draw();

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  std::println("[INFO] Stopping...");

  return 0;
}
void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) || glfwGetKey(window, GLFW_KEY_Q)) {
    glfwSetWindowShouldClose(window, true);
  }

  using dir = derp::camera::direction;
  const auto *cs =
      static_cast<CameraSystem *>(glfwGetWindowUserPointer(window));

  if (cs->gamepad_present) {
    glfwGetGamepadState(GLFW_JOYSTICK_1, &cs->gamepad);
    const auto &[buttons, axes] = cs->gamepad;

    constexpr float threshold = 0.01f;

    auto left_axis_x = axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    if (std::abs(left_axis_x) < threshold)
      left_axis_x = 0.0f;

    auto left_axis_y = axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
    if (std::abs(left_axis_y) < threshold)
      left_axis_y = 0.0f;

    auto right_axis_x = axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    if (std::abs(right_axis_x) < threshold)
      right_axis_x = 0.0f;

    auto right_axis_y = axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
    if (std::abs(right_axis_y) < threshold)
      right_axis_y = 0.0f;

    cs->camera.gamepad_move(left_axis_x, left_axis_y, right_axis_x,
                            right_axis_y, cs->delta_time);

    if (buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }
  }

  if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP)) {
    cs->camera.keyboard_move(dir::FORWARD, cs->delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT)) {
    cs->camera.keyboard_move(dir::LEFT, cs->delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN)) {
    cs->camera.keyboard_move(dir::BACKWARD, cs->delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT)) {
    cs->camera.keyboard_move(dir::RIGHT, cs->delta_time);
  }
}
void mouse_callback(GLFWwindow *window, const double x, const double y) {
  const auto *cs =
      static_cast<CameraSystem *>(glfwGetWindowUserPointer(window));

  const auto x_pos = static_cast<float>(x);
  const auto y_pos = static_cast<float>(y);

  if (cs->first_mouse) {
    cs->last_x = x_pos;
    cs->last_y = y_pos;
    cs->first_mouse = false;
  }

  const float x_off = x_pos - cs->last_x;
  const float y_off = cs->last_y - y_pos; // y axis is flipped!

  cs->last_x = x_pos;
  cs->last_y = y_pos;

  cs->camera.mouse_move(x_off, y_off);
}
void scroll_callback(GLFWwindow *window, const double x_off,
                     const double y_off) {
  const auto *cs =
      static_cast<CameraSystem *>(glfwGetWindowUserPointer(window));
  cs->camera.mouse_scroll(static_cast<float>(y_off));
}

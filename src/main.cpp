//===-- Main executable for derp ------------------------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#include "derp/camera.hpp"
#include "derp/texture.hpp"

#include <derp/shader.hpp>

#include <cstdint>
#include <iostream>
#include <ostream>
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

  constexpr float vertices[24 * 8] = {
      // position          normal               uv
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, // front
      -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f, // front
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, // front
      0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f, // front
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, // back
      0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, // back
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, // back
      -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, // back
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f, // top
      -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top
      0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // top
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 0.0f, // bottom
      0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, // bottom
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 1.0f, // bottom
      -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, // bottom
      -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f, // left
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, // left
      -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f, // left
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, // left
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // right
      0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // right
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // right
      0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // right
  };

  // memory saved = 12 * 8 * 4 - 36 * 4
  //              = 240 bytes per cube
  constexpr uint32_t indices[36] = {
      0,  1,  2,  // front
      0,  2,  3,  // front
      4,  5,  6,  // back
      4,  6,  7,  // back
      8,  9,  10, // top
      8,  10, 11, // top
      12, 13, 14, // bottom
      12, 14, 15, // bottom
      16, 17, 18, // left
      16, 18, 19, // left
      20, 21, 22, // right
      20, 22, 23, // right
  };

  uint32_t vao;
  glCreateVertexArrays(1, &vao);

  uint32_t vbo;
  glCreateBuffers(1, &vbo);
  glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);

  uint32_t ibo;
  glCreateBuffers(1, &ibo);
  glNamedBufferData(ibo, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, 8 * sizeof(float));
  glVertexArrayElementBuffer(vao, ibo);

  glEnableVertexArrayAttrib(vao, 0);
  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, 0);
  glVertexArrayAttribBinding(vao, 0, 0);

  glEnableVertexArrayAttrib(vao, 1);
  glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, false, 6 * sizeof(float));
  glVertexArrayAttribBinding(vao, 1, 0);

  {
    derp::shader s(RESOURCES_PATH "/shaders/transform.vert",
                   RESOURCES_PATH "/shaders/texture.frag");
    s.use();

    auto model = glm::identity<glm::mat4>();

    const auto projection =
        glm::perspective(glm::radians(cs.camera.get_fov()),
                         static_cast<float>(WIDTH) / HEIGHT, 0.1f, 100.0f);
    s["u_projection"] = projection;

    derp::texture t(RESOURCES_PATH "/textures/container.png");
    t.use();

    glBindVertexArray(vao);

    while (!glfwWindowShouldClose(window)) {
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      const auto current_frame = static_cast<float>(glfwGetTime());
      cs.delta_time = current_frame - cs.last_frame;
      cs.last_frame = current_frame;

      process_input(window);

      s["u_model"] = model;
      s["u_view"] = cs.camera.get_view_matrix();

      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

  glDeleteBuffers(1, &ibo);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

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

    return;
  }

  // cs->camera.gamepad_move(1.0f, 0.0f, 0.0f, 0.0f, cs->delta_time);

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

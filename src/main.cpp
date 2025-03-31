//===-- Main executable for derp ------------------------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#include "derp/texture.hpp"

#include <derp/shader.hpp>

#include <cstdint>
#include <print>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow *window, const int width,
                               const int height) {
  glViewport(0, 0, width, height);
}

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

  GLFWwindow *window = glfwCreateWindow(640, 480, "derp", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    std::println("[ERROR] Couldn't create GLFW window.");
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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
    s["u_projection"] = glm::identity<glm::mat4>();
    s["u_view"] = glm::identity<glm::mat4>();

    derp::texture t(RESOURCES_PATH "/textures/container.png");
    t.use();

    glBindVertexArray(vao);

    while (!glfwWindowShouldClose(window)) {
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      s["u_model"] = model;
      const auto theta = std::sin(static_cast<float>(glfwGetTime()));
      model =
          glm::rotate(model, glm::radians(theta), glm::vec3(1.0f, 0.3f, 0.5f));

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

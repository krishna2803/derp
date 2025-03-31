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

  constexpr float vertices[] = {
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
  };

  constexpr uint32_t indices[] = {0, 1, 3, 1, 2, 3};

  uint32_t vao;
  glCreateVertexArrays(1, &vao);

  uint32_t vbo;
  glCreateBuffers(1, &vbo);
  glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);

  uint32_t ibo;
  glCreateBuffers(1, &ibo);
  glNamedBufferData(ibo, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, 5 * sizeof(float));
  glVertexArrayElementBuffer(vao, ibo);

  glEnableVertexArrayAttrib(vao, 0);
  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, 0);
  glVertexArrayAttribBinding(vao, 0, 0);

  glEnableVertexArrayAttrib(vao, 1);
  glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, false, 3 * sizeof(float));
  glVertexArrayAttribBinding(vao, 1, 0);

  {
    derp::shader s(RESOURCES_PATH "/shaders/texture.vert",
                   RESOURCES_PATH "/shaders/texture.frag");
    s.use();

    derp::texture t(RESOURCES_PATH "/textures/container.png");
    t.use();

    glBindVertexArray(vao);

    while (!glfwWindowShouldClose(window)) {
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

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

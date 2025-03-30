//===-- Main executable for derp ------------------------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#include "shader.hpp"

#include <cstdint>
#include <iostream>
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
    std::println(std::cerr, "[ERROR] Couldn't initialize GLFW.");
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
    std::println(std::cerr, "[ERROR] Couldn't create GLFW window.");
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::println(std::cerr, "[ERROR] Couldn't initialize GLAD.");
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
      -0.5f, -0.5f, 0.0f, // Bottom-left
      0.5f,  -0.5f, 0.0f, // Bottom-right
      0.0f,  0.5f,  0.0f  // Top-center
  };

  uint32_t vao;
  glCreateVertexArrays(1, &vao);

  uint32_t vbo;
  glCreateBuffers(1, &vbo);
  glNamedBufferStorage(vbo, sizeof(vertices), vertices, 0);

  glVertexArrayVertexBuffer(vao, 0, vbo, 0, 3 * sizeof(float));

  glEnableVertexArrayAttrib(vao, 0);
  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, false, 0);
  glVertexArrayAttribBinding(vao, 0, 0);

  {
    derp::shader s(RESOURCES_PATH "/shaders/simple.vert",
                   RESOURCES_PATH "/shaders/simple.frag");
    s.use();

    // int loc = s["u_color"];
    // std::println("loc = {}", loc);

    s["u_color"] = glm::vec3(1.0f, 0.1f, 0.1f);

    glBindVertexArray(vao);

    while (!glfwWindowShouldClose(window)) {
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glDrawArrays(GL_TRIANGLES, 0, 3);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  glfwDestroyWindow(window);
  glfwTerminate();

  std::println("[INFO] Stopping...");

  return 0;
}

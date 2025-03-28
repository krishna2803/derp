#include <iostream>
#include <print>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
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

  const auto vertex_src = R"(#version 460 core
                              layout(location=0) in vec3 a_pos;
                              void main() {
                                  gl_Position = vec4(a_pos, 1.0);
                              })";

  const auto fragment_src = R"(#version 460 core
                             out vec4 frag_color;
                             void main() {
                               frag_color = vec4(1.0);
                             })";

  const uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
  const uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vs, 1, &vertex_src, nullptr);
  glCompileShader(vs);

  glShaderSource(fs, 1, &fragment_src, nullptr);
  glCompileShader(fs);

  const uint32_t program = glCreateProgram();

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);
  glValidateProgram(program);

  glDetachShader(program, vs);
  glDetachShader(program, fs);

  glDeleteShader(vs);
  glDeleteShader(fs);

  glUseProgram(program);
  glBindVertexArray(vao);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteProgram(program);

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  glfwDestroyWindow(window);
  glfwTerminate();

  std::println("[INFO] Stopping...");

  return 0;
}
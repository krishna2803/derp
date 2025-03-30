//===-- Implementation of shader class ------------------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#include "shader.hpp"

#include <fstream>
#include <iostream>
#include <print> // std::print

namespace derp {

shader::shader(const std::string &vert_path, const std::string &frag_path)
    : deleted(false) {
  id = 0;

  auto read_file = [](const std::string &path) -> std::string {
    if (std::ifstream file{path, std::ios::binary | std::ios::ate}) {
      const auto size = file.tellg();
      std::string content;
      content.resize(size);
      file.seekg(0);
      if (!file.read(content.data(), size)) {
        throw std::runtime_error(
            std::format("[ERROR] Failed to read file: {}.", path));
      }
      return content; // NRVO
    }
    throw std::runtime_error(
        std::format("[ERROR] Couldn't open file: {}.", path));
  };

  const auto vert_str = read_file(vert_path);
  const auto frag_str = read_file(frag_path);

  const char *vert_src = vert_str.c_str();
  const char *frag_src = frag_str.c_str();

  std::println("[DEBUG] Vertex Shader Source: {}", vert_src);
  std::println("[DEBUG] Fragment Shader Source: {}", frag_src);

  const uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
  const uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vs, 1, &vert_src, nullptr);
  glCompileShader(vs);

  int result;
  char info_log[1024];
  glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
  if (!result) {
    glGetShaderInfoLog(vs, 1024, nullptr, info_log);
    throw std::runtime_error(
        std::format("[ERROR] Vertex Shader Compilation Failed.\n{}", info_log));
  }

  glShaderSource(fs, 1, &frag_src, nullptr);
  glCompileShader(fs);

  glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
  if (!result) {
    glGetShaderInfoLog(fs, 1024, nullptr, info_log);
    throw std::runtime_error(std::format(
        "[ERROR] Fragment Shader Compilation Failed.\n{}", info_log));
  }

  id = glCreateProgram();

  glAttachShader(id, vs);
  glAttachShader(id, fs);

  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(id, 1024, nullptr, info_log);
    throw std::runtime_error(
        std::format("[ERROR] Program Link Failed.\n{}", info_log));
  }

  glValidateProgram(id);

  glGetProgramiv(id, GL_VALIDATE_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(id, 1024, nullptr, info_log);
    throw std::runtime_error(
        std::format("[ERROR] Program Validation Failed.\n{}", info_log));
  }

  glDetachShader(id, vs);
  glDetachShader(id, fs);

  glDeleteShader(vs);
  glDeleteShader(fs);

  std::println("[DEBUG] Program with id = {} successfully created.", id);
}
shader::~shader() {
  std::println("[DEBUG] attempting to delete program with id = {}", id);
  if (deleted)
    return;
  glUseProgram(0);
  if (glIsProgram(id))
    glDeleteProgram(id);
  deleted = true;
  std::println("[DEBUG] program with id = {} deleted.", id);
}

inline shader::shader(shader &&other) noexcept
    : id(std::exchange(other.id, 0)),
      deleted(std::exchange(other.deleted, true)),
      uniform_map(std::move(other.uniform_map)) {}

inline shader &shader::operator=(shader &&other) noexcept {
  if (this != &other) {
    if (!deleted && glIsProgram(id)) {
      glDeleteProgram(id);
    }
    id = std::exchange(other.id, 0);
    deleted = std::exchange(other.deleted, true);
    uniform_map = std::move(other.uniform_map); // Move the map contents
  }
  return *this;
}

void shader::use() const {
  if (deleted) {
    throw std::runtime_error(std::format(
        "[ERROR ] attempted to use deleted shader program with id = {}.", id));
  }
  glUseProgram(id);
  std::println("[DEBUG] program with id = {} used.", id);
}
std::string shader::UniformProxy::get_uniform_name_by_location(uint32_t,
                                                               int loc) {
  return std::format("uniform @ location {}", loc);
}

[[nodiscard]] int shader::get_uniform_location(std::string_view name) const {
  if (deleted) {
    throw std::runtime_error(std::format(
        "Attempted to get uniform location '{}' on deleted shader program",
        name));
  }

  if (const auto it = uniform_map.find(name); it != uniform_map.end()) {
    return it->second;
  }

  if (!glIsProgram(id)) {
    throw std::runtime_error(
        std::format("Attempted to get uniform location '{}' on invalid shader "
                    "program (id was {})",
                    name, id));
  }
  const int location = glGetUniformLocation(id, name.data());

  if (location == -1) {
    throw std::runtime_error(std::format(
        "[ERROR] uniform '{}' not found in shader program {}", name, id));
  }

  uniform_map.emplace(std::string(name), location);
  return location;
}

template <UniformType T>
const shader::UniformProxy &
shader::UniformProxy::operator=(const T &value) const {
  int current_program = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
  if (static_cast<uint32_t>(current_program) != program_id) {
    throw std::runtime_error(std::format(
        "Attempted to set uniform for shader program {} ('{}') "
        "when program {} is active.",
        program_id, get_uniform_name_by_location(program_id, location),
        current_program));
  }

  // the get location thing should through runtime error, but nvm :)
  assert(location != -1);

  using DecayedT = std::decay_t<T>;

  if constexpr (std::same_as<DecayedT, float>) {
    glUniform1f(location, value);
  } else if constexpr (std::same_as<DecayedT, int>) {
    glUniform1i(location, value);
  } else if constexpr (std::same_as<DecayedT, bool>) {
    glUniform1i(location, static_cast<int>(value));
  } else if constexpr (std::same_as<DecayedT, glm::vec2>) {
    glUniform2fv(location, 1, glm::value_ptr(value));
  } else if constexpr (std::same_as<DecayedT, glm::vec3>) {
    glUniform3fv(location, 1, glm::value_ptr(value));
  } else if constexpr (std::same_as<DecayedT, glm::vec4>) {
    glUniform4fv(location, 1, glm::value_ptr(value));
  } else if constexpr (std::same_as<DecayedT, glm::mat3>) {
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
  } else if constexpr (std::same_as<DecayedT, glm::mat4>) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
  }

  // TODO: check for OpenGL errors

  return *this;
}

} // namespace derp
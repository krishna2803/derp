//===-- Implementation header for shader class ----------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "glm/gtc/type_ptr.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <format>
#include <glad/glad.h>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace derp {

template <typename T>
concept UniformType = requires(T value, int location) {
  requires std::same_as<T, float> || std::same_as<T, int> ||
               std::same_as<T, bool> || std::same_as<T, glm::vec2> ||
               std::same_as<T, glm::vec3> || std::same_as<T, glm::vec4> ||
               std::same_as<T, glm::mat3> || std::same_as<T, glm::mat4>;
};

class shader {
public:
  shader() = delete;
  shader(const std::string &vert_path, const std::string &frag_path);
  // shader(const std::string &vert_path, const std::string &frag_path,
  // const std::string &geom_path);

  ~shader();

  void use() const;

  template <typename T>
    requires std::convertible_to<T, std::string_view>
  auto operator[](T &&name) const -> int {
    const int location = get_uniform_location(std::forward<T>(name));
    return location;
  }

private:
  uint32_t id;
  bool deleted;
  mutable std::unordered_map<std::string, int> uniform_map;

  [[nodiscard]] int get_uniform_location(const std::string_view name) const {
    std::string name_str(name);
    if (const auto it = uniform_map.find(name_str); it != uniform_map.end()) {
      return it->second;
    }

    const int location = glGetUniformLocation(id, name_str.c_str());
    if (location == -1) {
      throw std::runtime_error(std::format("Uniform '{}' not found", name));
    }
    uniform_map.emplace(name_str, location);
    return location;
  }

  class UniformProxy {
    uint32_t program;
    int location;

  public:
    UniformProxy(const uint32_t prog, const int loc)
        : program(prog), location(loc) {}

    template <UniformType T> auto operator=(T &&value) const {
      if constexpr (std::same_as<std::decay_t<T>, float>) {
        glUniform1f(location, value);
      } else if constexpr (std::same_as<std::decay_t<T>, int>) {
        glUniform1i(location, value);
      } else if constexpr (std::same_as<std::decay_t<T>, bool>) {
        glUniform1i(location, static_cast<int>(value));
      } else if constexpr (std::same_as<std::decay_t<T>, glm::vec2>) {
        glUniform2fv(location, 1, glm::value_ptr(value));
      } else if constexpr (std::same_as<std::decay_t<T>, glm::vec3>) {
        glUniform3fv(location, 1, glm::value_ptr(value));
      } else if constexpr (std::same_as<std::decay_t<T>, glm::vec4>) {
        glUniform4fv(location, 1, glm::value_ptr(value));
      } else if constexpr (std::same_as<std::decay_t<T>, glm::mat3>) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
      } else if constexpr (std::same_as<std::decay_t<T>, glm::mat4>) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
      }
      return *this;
    }
  };
};

} // namespace derp

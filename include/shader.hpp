//===-- Implementation header for shader class ----------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cassert>       // assert
#include <concepts>      // std::same_as, std::convertible_to
#include <cstdint>       // uint32_t
#include <format>        // std::format
#include <functional>    // std::hash, std::equal_to
#include <stdexcept>     // std::runtime_error
#include <string>        // std::string
#include <string_view>   // std::string_view
#include <type_traits>   // std::decay_t
#include <unordered_map> // std::unordered_map
#include <utility>       // std::forward, std::move, std::exchange

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace derp {

template <typename T>
concept UniformType = requires {
  requires std::same_as<std::decay_t<T>, float> ||
               std::same_as<std::decay_t<T>, int> ||
               std::same_as<std::decay_t<T>, bool> ||
               std::same_as<std::decay_t<T>, glm::vec2> ||
               std::same_as<std::decay_t<T>, glm::vec3> ||
               std::same_as<std::decay_t<T>, glm::vec4> ||
               std::same_as<std::decay_t<T>, glm::mat3> ||
               std::same_as<std::decay_t<T>, glm::mat4>;
};

struct StringViewHash {
  using is_transparent = void;
  [[nodiscard]] std::size_t
  operator()(const std::string_view sv) const noexcept {
    return std::hash<std::string_view>{}(sv);
  }
  [[nodiscard]] std::size_t operator()(const std::string &s) const noexcept {
    return std::hash<std::string>{}(s);
  }
  [[nodiscard]] std::size_t operator()(const char *cstr) const noexcept {
    return std::hash<std::string_view>{}(cstr);
  }
};

class shader {
public:
  shader() = delete;
  shader(const std::string &vert_path, const std::string &frag_path);
  // shader(const std::string &vert_path, const std::string &frag_path,
  // const std::string &geom_path);

  shader(const shader &) = delete;
  shader &operator=(const shader &) = delete;
  shader(shader &&other) noexcept;
  shader &operator=(shader &&other) noexcept;

  ~shader();

  auto use() const;

  class UniformProxy {
    friend class shader;

    uint32_t program_id;
    int location;

    UniformProxy(const uint32_t prog_id, const int loc)
        : program_id(prog_id), location(loc) {
    } // Don't allow default construction or copying

  public:
    UniformProxy() = delete;
    UniformProxy(const UniformProxy &) = delete;
    UniformProxy &operator=(const UniformProxy &) = delete;
    UniformProxy(UniformProxy &&) = default;
    UniformProxy &operator=(UniformProxy &&) = default;

    template <UniformType T>
    auto operator=(const T &value) const -> const UniformProxy &;

    operator int() const { return location; }

    // Note: This method doesn't actually query OpenGL for the name.
    // It just provides a descriptive string based on the location.
    static auto get_uniform_name_by_location(uint32_t /*prog_id*/, int loc)
        -> std::string;
  }; // UniformProxy

  template <typename T>
    requires std::convertible_to<T, std::string_view>
  [[nodiscard]]
  auto operator[](T &&name) const -> UniformProxy {
    const int location =
        get_uniform_location(std::string_view(std::forward<T>(name)));
    return {id, location};
  }

private:
  uint32_t id = 0;
  bool deleted = true;

  using UniformMap =
      std::unordered_map<std::string, int,
                         StringViewHash, // Use our transparent hash
                         std::equal_to<> // Use transparent equality
                                         // (std::equal_to<void>)
                         >;
  mutable UniformMap uniform_map;

  [[nodiscard]] int get_uniform_location(std::string_view name) const;

}; // shader

template <UniformType T>
auto shader::UniformProxy::operator=(const T &value) const
    -> const shader::UniformProxy & {
  int current_program = 0;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
  if (static_cast<uint32_t>(current_program) != program_id) {
    throw std::runtime_error(std::format(
        "Attempted to set uniform for shader program {} ('{}') "
        "when program {} is active.",
        program_id, get_uniform_name_by_location(program_id, location),
        current_program));
  }

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
  // TODO: Add glGetError check here?
  return *this;
}

} // namespace derp

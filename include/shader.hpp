//===-- Implementation header for shader class ----------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <concepts>      // std::same_as, std::convertible_to
#include <cstdint>       // uint32_t
#include <format>        // std::format
#include <functional>    // std::hash, std::equal_to
#include <stdexcept>     // std::runtime_error
#include <string>        // std::string
#include <string_view>   // string_view
#include <type_traits>   // std::decay_t
#include <unordered_map> // std::unordered_map
#include <utility>       // For std::forward, std::move, std::exchange

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
private:
  class UniformProxy;

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

  void use() const;

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

  class UniformProxy {
    friend class shader;

    uint32_t program_id;
    int location;

    UniformProxy(const uint32_t prog_id, const int loc)
        : program_id(prog_id), location(loc) {}

  public:
    template <UniformType T>
    const UniformProxy &operator=(const T &value) const;

    operator int() const { return location; }

    static std::string get_uniform_name_by_location(uint32_t /*prog_id*/,
                                                    int loc);
  }; // UniformProxy
}; // shader

} // namespace derp

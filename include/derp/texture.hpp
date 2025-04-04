//===-- Implementation header for texture class ---------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <string>

namespace derp {
class texture {
public:
  enum class texture_type { AMBIENT, DIFFUSE, SPECULAR, NORMAL, HEIGHT };

private:
  uint32_t id = 0;
  bool deleted = true;
  texture_type _type;

public:
  explicit texture(const std::string &texture_path,
                   texture_type _type = texture_type::DIFFUSE);

  ~texture();

  auto use(uint32_t unit = 0) const -> void;

}; // class texture
} // namespace derp

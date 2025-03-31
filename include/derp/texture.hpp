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
private:
  uint32_t id = 0;
  bool deleted = true;

public:
  explicit texture(const std::string &texture_path);

  ~texture();

  auto use(uint32_t unit = 0) const -> void;

}; // class texture
} // namespace derp

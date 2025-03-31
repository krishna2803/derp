//===-- Implementation for texture class ----------------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#include "texture.hpp"

#include <format>
#include <glad/glad.h>
#include <print>
#include <stb/stb_image.h>
#include <stdexcept>

namespace derp {

texture::texture(const std::string &texture_path) {
  int w, h, n;
  stbi_set_flip_vertically_on_load(true);

  unsigned char *data = stbi_load(texture_path.data(), &w, &h, &n, 0);

  if (!data) {
    throw std::runtime_error(
        std::string(std::format("[ERROR] file not found {}", texture_path)));
  }

  glCreateTextures(GL_TEXTURE_2D, 1, &id);

  glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  const auto img_format = (n == 4) ? GL_RGBA : GL_RGB;
  const auto gl_format = (n == 4) ? GL_RGBA8 : GL_RGB8;

  glTextureStorage2D(id, 1, gl_format, w, h);
  glTextureSubImage2D(id, 0, 0, 0, w, h, img_format, GL_UNSIGNED_BYTE, data);

  stbi_image_free(data);
  deleted = false;
}
texture::~texture() {
  if (deleted)
    return;
  if (glIsTexture(id)) {
    deleted = true;
    glDeleteTextures(1, &id);
    // std::println("[DEBUG] texture with id = {} deleted", id);
  }
}

auto texture::use(const uint32_t unit) const -> void {
  glBindTextureUnit(unit, id);
}

} // namespace derp

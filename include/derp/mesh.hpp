//===-- Implementation header for mesh class ------------------------------===//
//
// Copyright (c) 2025 Krishna Pandey. All rights reserved.
// SPDX-License-Identifier: MIT
// Part of the derp project, under the MIT License.
// See https://opensource.org/licenses/MIT for license information.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <glad/glad.h>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include "rapidobj/rapidobj.hpp"

#include <format>
#include <print>
#include <vector>

namespace derp {

constexpr float cube_vertices[24 * 8] = {
    // position          normal               uv
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, // front
    -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f, // front
    0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, // front
    0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f, // front
    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, // back
    0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, // back
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, // back
    -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, // back
    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f, // top
    -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, // top
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, // top
    0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 0.0f, // top
    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 0.0f, // bottom
    0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, // bottom
    0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 1.0f, // bottom
    -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, // bottom
    -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f, // left
    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, // left
    -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f, // left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, // left
    0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 0.0f, // right
    0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, // right
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, // right
    0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, // right
};

// memory saved = 12 * 8 * 4 - 36 * 4
//              = 240 bytes per cube
constexpr uint32_t cube_indices[36] = {
    0,  1,  2,  // front
    0,  2,  3,  // front
    4,  5,  6,  // back
    4,  6,  7,  // back
    8,  9,  10, // top
    8,  10, 11, // top
    12, 13, 14, // bottom
    12, 14, 15, // bottom
    16, 17, 18, // left
    16, 18, 19, // left
    20, 21, 22, // right
    20, 22, 23, // right
};

class mesh {
public:
  struct vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    [[nodiscard]] std::string to_str() const {
      return std::format(
          "vertex(position=({:.2f}, {:.2f}, {:.2f}), normal=({:.2f}, {:.2f}, "
          "{:.2f}), uv=({:.2f}, {:.2f}))",
          position.x, position.y, position.z, normal.x, normal.y, normal.z,
          uv.x, uv.y);
    }
  };

private:
  std::vector<vertex> vertices;
  std::vector<uint32_t> indices;

  uint32_t vao{};
  uint32_t vbo{};
  uint32_t ibo{};

public:
  mesh(const mesh &) = delete;
  mesh &operator=(const mesh &) = delete;

  mesh(std::vector<vertex> &&vertices, std::vector<uint32_t> &&indices)
      : vertices(std::move(vertices)), indices(std::move(indices)) {
    glCreateVertexArrays(1, &vao);

    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo,
                      static_cast<int>(this->vertices.size() * sizeof(vertex)),
                      this->vertices.data(), GL_STATIC_DRAW);

    glCreateBuffers(1, &ibo);
    glNamedBufferData(ibo,
                      static_cast<int>(this->indices.size() * sizeof(uint32_t)),
                      this->indices.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(vertex));
    glVertexArrayElementBuffer(vao, ibo);

    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);

    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE,
                              offsetof(vertex, normal));
    glVertexArrayAttribBinding(vao, 1, 0);

    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE,
                              offsetof(vertex, uv));
    glVertexArrayAttribBinding(vao, 2, 0);
  }

  ~mesh() {
    if (ibo)
      glDeleteBuffers(1, &ibo);
    if (vbo)
      glDeleteBuffers(1, &vbo);
    if (vao)
      glDeleteVertexArrays(1, &vao);
  }

  void use() const { glBindVertexArray(vao); }

  void draw() const {
    glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()),
                   GL_UNSIGNED_INT, nullptr);
  }

  void use_and_draw() const {
    use();
    draw();
  }

  static auto from_obj(const std::string &filepath) {
    auto result = rapidobj::ParseFile(filepath);

    if (result.error) {
      throw std::runtime_error(
          std::format("[ERROR] {} {}", filepath, result.error.code.message()));
    }

    if (const bool success = rapidobj::Triangulate(result); !success) {
      throw std::runtime_error(
          std::format("[ERROR] {}:", result.error.code.message()));
    }

    size_t num_triangles{};
    for (const auto &shape : result.shapes) {
      num_triangles += shape.mesh.num_face_vertices.size();
    }

    std::println("n_shapes: {}", result.shapes.size());
    std::println("n_materials: {}", result.materials.size());
    std::println("n_triangles: {}", num_triangles);
  }
}; // class mesh

} // namespace derp

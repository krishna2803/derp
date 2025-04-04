#version 460 core

layout(location=0) in vec3 a_pos;
layout(location=1) in vec2 a_nrm;
layout(location=2) in vec2 a_tex;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;

out vec2 tex_coord;

void main() {
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);
    tex_coord = a_tex;
}

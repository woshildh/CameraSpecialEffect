#version 300 es

// 顶点位置
layout(location = 2) in vec4 vert_pos;

uniform mat4 mvp_mat;

out vec2 tex_coord;

void main() {
    gl_Position = mvp_mat * vec4(vert_pos.x, vert_pos.y, 0.0, 1.0);
    tex_coord = vert_pos.zw;
}

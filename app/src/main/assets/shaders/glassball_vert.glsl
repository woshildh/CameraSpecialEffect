#version 300 es

layout(location = 1) in vec3 vert_pos;
layout(location = 2) in vec2 tex_coord;

// 变换矩阵
uniform mat4 transform_mat;

// 输出
out vec2 tx;

void main() {
    gl_Position = transform_mat * vec4(vert_pos, 1.0);
//    gl_Position = vec4(vert_pos, 1.0);
    tx = tex_coord;
}

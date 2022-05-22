#version 300 es

layout(location = 1) in vec4 vert_pos;
uniform mat4 transform_mat;   // 平移，旋转，缩放矩阵等组成的变换矩阵
out vec2 tex_coord;

void main() {
    gl_Position = transform_mat * vec4(vert_pos.x, vert_pos.y, 0.0, 1.0);
    tex_coord = vert_pos.zw;
}

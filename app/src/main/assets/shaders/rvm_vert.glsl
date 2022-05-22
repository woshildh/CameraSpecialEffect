#version 300 es

layout(location = 1) in vec4 vert_pos;
out vec2 tx;

void main() {
    gl_Position = vec4(vert_pos.x, vert_pos.y, 0.0, 1.0);
    tx = vert_pos.zw;
}

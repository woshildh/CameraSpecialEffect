#version 300 es

layout(location = 1) in vec2 vert_pos;

void main() {
    gl_Position = vec4(vert_pos.xy, 0.0, 1.0);
}

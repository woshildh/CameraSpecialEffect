#version 300 es

layout(location = 2) in vec2 vert_pos;
layout(location = 3) in vec3 vert_color;
layout(location = 4) in float radius;

out vec3 vcolor;

void main() {
    gl_Position = vec4(vert_pos.x, vert_pos.y, 0.0, 1.0);
    gl_PointSize = radius;
    vcolor = vert_color;
}

#version 300 es

// 颜色
uniform vec3 color;
// 宽度
uniform float width;

out vec4 out_color;

void main() {
    out_color = vec4(color.xyz / 255.0, 0.8);
}

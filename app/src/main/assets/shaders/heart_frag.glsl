#version 300 es

layout(binding = 4) uniform sampler2D samp;
out vec4 out_color;
in vec2 tex_coord;

void main() {
    out_color = texture(samp, tex_coord);
}

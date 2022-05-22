#version 300 es

layout(binding = 1) uniform sampler2D samp;
in vec2 tx;
out vec4 out_color;

void main() {
    out_color = texture(samp, tx);
    out_color.a = 0.5;
}

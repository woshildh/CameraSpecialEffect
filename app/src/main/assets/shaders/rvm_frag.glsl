#version 300 es

layout(binding = 3) uniform sampler2D fore_tex;
layout(binding = 4) uniform sampler2D alpha_tex;
in vec2 tx;
out vec4 out_color;

void main() {
    out_color = texture(fore_tex, tx);
    vec4 alpha = texture(alpha_tex, tx);
    // 过滤掉不太确定的地方
    if(alpha.r < 0.05)
        alpha.r = 0.0;
    out_color.a = 1.0 - alpha.r;
}

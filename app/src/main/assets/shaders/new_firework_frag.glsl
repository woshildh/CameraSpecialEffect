#version 300 es

// 上升期的颜色
uniform vec3 rise_color;
// 上升期的纹理
layout(binding = 3) uniform sampler2D rise_samp;
// 爆炸期的纹理
layout(binding = 4) uniform sampler2D explode_samp;
// 当前阶段
uniform int stage;
// 纹理的位置
in vec2 tex_coord;
// 输出颜色
out vec4 out_color;


void main() {
    // 上升期
    if(stage == 0) {
        vec4 color = texture(rise_samp, tex_coord);
        out_color = vec4(rise_color / 255.0, 0.0);
        out_color.a = color.a;
    } else { // 爆炸期
        out_color = texture(explode_samp, tex_coord);
    }
}

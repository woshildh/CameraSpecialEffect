#version 300 es

layout(binding = 0) uniform sampler2D y_samp;
layout(binding = 1) uniform sampler2D u_samp;
layout(binding = 2) uniform sampler2D v_samp;

in vec2 tex_coord;
out vec4 out_color;

void main()
{
    vec3 yuv;
    yuv.r = texture(y_samp, tex_coord).r;
    yuv.g = texture(u_samp, tex_coord).r - 0.5;
    yuv.b = texture(v_samp, tex_coord).r - 0.5;
    vec3 rgb = mat3(1.0, 1.0, 1.0,
                0, -0.344, 1.770,
                1.403, -0.714, 0) * yuv;
    out_color = vec4(rgb, 1.0);
}

#version 300 es

layout(binding = 0) uniform  sampler2D y_plane;
layout(binding = 1) uniform  sampler2D u_plane;
layout(binding = 2) uniform  sampler2D v_plane;

// 纹理图像的尺寸
uniform vec2 tex_size;

// sigmaL 和 sigmaS
const float sigmaL = 20.0;  // 位置空间的方差
const float sigmaS = 20.0;  // 颜色空间的方差
const int filterSize = int(sigmaS) * 2;  // 滤波核的大小

// 调高亮度值
const float y_highten = 0.1;

// 输入的纹理坐标
in vec2 tex_coord;
// 输出的颜色
out vec4 out_color;

// 最小的图像滤波尺寸
#define EPS 1e-5

const mat3 color_trans_mat = mat3(
    1.0, 1.0, 1.0,
    0, -0.344, 1.770,
    1.403, -0.714, 0
);

vec3 get_color(vec2 tx)
{
    vec3 yuv;
    yuv.r =texture(y_plane, tx).x;
    yuv.g = texture(u_plane, tx).x - 0.5;
    yuv.b = texture(v_plane, tx).x - 0.5;
    vec3 rgb = color_trans_mat * yuv;
    return rgb;
}

void main()
{
    float facL = -1.0 / (2.0 * sigmaL * sigmaL);
    float facS = -1.0 / (2.0 * sigmaS * sigmaS);
    // 中心点颜色
    vec3 center_color = get_color(tex_coord);
    float mode_center = length(center_color);
    // 初始化权重
    float sumW = 0.0;
    vec4 sumC = vec4(0.0);
    for(int i = filterSize / 2; i <= filterSize; ++i) {
        for(int j = filterSize / 2; j <= filterSize; ++j) {
            vec2 pt_pos = vec2(float(i), float(j)) / tex_size + tex_coord;
            vec3 pt_color = get_color(pt_pos);
            // 计算距离
            float distL = length(pt_pos);
            float distS = length(pt_color) - mode_center;
            // 计算权重
            float wL = exp(facL * distL * distL);
            float wS = exp(facS * distS * distS);
            float w = wS * wL;
            // 加权
            sumW += w;
            sumC += vec4(pt_color * w, 1.0);
        }
    }
    out_color = sumC / sumW;
}


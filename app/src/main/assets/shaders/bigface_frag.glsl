#version 300 es

layout(binding = 0) uniform sampler2D samp_y;
layout(binding = 1) uniform sampler2D samp_u;
layout(binding = 2) uniform sampler2D samp_v;

in vec2 tex_coord;      // 纹理采样的坐标

uniform int has_face;   // 是否有人脸
uniform vec2 img_size;  // 图像尺寸
uniform vec4 face_rect;  // 人脸的框

out vec4 out_color;

vec3 get_color_by_tex(vec2 tx) {
    vec3 yuv;
    yuv.r = texture(samp_y, tx).r;
    yuv.g = texture(samp_u, tx).r - 0.5;
    yuv.b = texture(samp_v, tx).r - 0.5;
    vec3 rgb = mat3(
        1.0, 1.0, 1.0,
        0, -0.344, 1.770,
        1.403, -0.714, 0) * yuv;
    return rgb;
}

float fs (float r, float rmax){
    //放大系数
    float a = 0.7;
    return (1.0-(r/rmax-1.0)*(r/rmax-1.0))*a;
}

vec2 newCoord(vec2 coord ,vec2 eye ,float rmax){
    vec2 p = coord;
    float r = distance (coord,eye);
    //改变顶点位置
    if(r<rmax)
    {
        float fsr = fs(r,rmax);
        //根据下面关系求p
        //(p-eye)/(coord-eye) = fsr/r;
        p = fsr*(coord - eye) + eye;
    }
    return p;
}

void main() {
    // 从yuv坐标获取rgb的颜色
    vec3 rgb = get_color_by_tex(tex_coord);
    // 如果没有人脸则直接返回
    if(has_face == 0) {
        out_color = vec4(rgb, 1.0);
        return;
    }
    // 计算人脸的中心点坐标
    float rmax = distance(face_rect.xy, face_rect.zw) * 2.5;
    vec2 center = (face_rect.xy + face_rect.zw) / 2.0;
    vec2 new_coord = newCoord(tex_coord, center, rmax);
    // 返回颜色
    out_color = vec4(get_color_by_tex(new_coord), 1.0);
}

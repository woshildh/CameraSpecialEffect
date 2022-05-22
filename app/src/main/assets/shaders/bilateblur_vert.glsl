#version 300 es

layout(location = 0) in vec4 position; // 顶点的坐标
uniform mat4 frame_rotation;       // 旋转的矩阵
out vec2 tex_coord;             // 纹理坐标

void main()
{
    gl_Position = frame_rotation * vec4(position.x, position.y, 0.0, 1.0);
    tex_coord = position.zw;
}

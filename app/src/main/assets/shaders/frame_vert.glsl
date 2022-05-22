#version 300 es
layout(location = 0) in vec4 vert_pos;
uniform mat4 frame_rotation;  // 旋转矩阵，因为画面是有旋转的
out vec2 tex_coord;

void main()
{
    gl_Position = frame_rotation * vec4(vert_pos.x, vert_pos.y, 0.0, 1.0);
    tex_coord = vec2(vert_pos.z, vert_pos.w);
}



#version 300 es

layout(location = 3) in vec2 pos;
uniform mediump int is_rect;

void main()
{
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
    if(is_rect == 0) {
        gl_PointSize = 10.0;
    }
}

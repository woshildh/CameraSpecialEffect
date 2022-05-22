#version 300 es

// 标志着是框还是关键点
uniform mediump int is_rect;

out vec4 out_color;

void main()
{
    if(is_rect == 0) {
        out_color = vec4(0.8, 0.0, 0.0, 1.0);
    } else {
        out_color = vec4(0.0, 0.8, 0.0, 1.0);
    }
}
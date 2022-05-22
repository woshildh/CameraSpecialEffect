#version 300 es

in vec3 vcolor;
out vec4 out_color;

void main() {
    out_color = vec4(vcolor / 255.0, 0.6);
//    out_color = vec4(1.0, 0.0, 0.0, 1.0);
}

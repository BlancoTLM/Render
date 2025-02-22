#version 410

out vec4 out_color;
in vec3 vertex_position;
in vec2 uv_position;

void main()
{
    out_color = vec4(uv_position, 1, 1.);
}
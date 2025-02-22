#version 410

out vec4 out_color;
in vec3 vertex_position;
in vec2 uv_position;
uniform sampler2D square_image;

void main()
{
    out_color = vec4(0, 1, 1,1);
    // out_color = texture(square_image, uv_position);
}
#version 410

out vec4 out_color;
in vec3 vertex_position;
in vec2 uv_position;
in vec3 out_normal;
uniform sampler2D square_image;
uniform vec3 light_direction;

void main()
{
    float light_intensity = dot(normalize(light_direction), out_normal);
    light_intensity = clamp(light_intensity, 0.2, 1);
    out_color = vec4(texture(square_image, uv_position).xyz * light_intensity, 1);
}
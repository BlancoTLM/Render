#version 410

out vec4 out_color;
in vec3 vertex_position;
in vec2 uv_position;
in vec3 out_normal;
uniform sampler2D square_image;
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec4 light_color;
uniform float point_light_intensity;

void main()
{
    float ambient_light_intensity = dot(normalize(light_direction), out_normal);
    ambient_light_intensity = clamp(ambient_light_intensity, 0.2, 1);

    vec4 color = vec4(texture(square_image, uv_position).xyz * ambient_light_intensity, 1);

    vec3 direction = light_position - vertex_position;
    float distance = length(direction);
    float attenuation = point_light_intensity / (distance * distance);
    attenuation = clamp(attenuation, 0, 1);
    color += attenuation * light_color;
    
    color = clamp(color, 0, 1);
    out_color = color;
}
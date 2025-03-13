#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
uniform float offset;
uniform mat4 view_projection_matrix;
uniform mat4 displacement_matrix;
out vec3 vertex_position;
out vec2 uv_position;
out vec3 out_normal;

vec3 apply_matrix_to_position(mat4 matrix, vec3 point)
{
    vec4 tmp = matrix * vec4(point, 1.);
    return tmp.xyz / tmp.w;
}

vec3 apply_matrix_to_direction(mat4 matrix, vec3 direction)
{
    vec4 tmp = matrix * vec4(direction, 0.);
    return normalize(tmp.xyz);
}

void main()
{
    vertex_position = apply_matrix_to_position(displacement_matrix, in_position);
    uv_position = in_uv;
    out_normal = apply_matrix_to_direction(displacement_matrix, in_normal);
    vec3 shaderPos = vec3(in_position.x + offset, in_position.y, in_position.z);
    gl_Position = view_projection_matrix * vec4(shaderPos, 1.0);
}
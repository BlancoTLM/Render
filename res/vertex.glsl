#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
uniform float offset;
uniform mat4 view_projection_matrix;
out vec3 vertex_position;
out vec2 uv_position;
out vec3 out_normal;

void main()
{
    vertex_position = in_position;
    uv_position = in_uv;
    out_normal = in_normal;
    vec3 shaderPos = vec3(in_position.x + offset, in_position.y, in_position.z);
    gl_Position = view_projection_matrix * vec4(shaderPos, 1.0);
}
#version 410

layout(location = 0) in vec3 in_position;
uniform float offset;
uniform mat4 view_projection_matrix;

void main()
{
    vec3 shaderPos = vec3(in_position.x + offset, in_position.y, in_position.z);
    gl_Position = view_projection_matrix  * vec4(shaderPos, 1.);
}
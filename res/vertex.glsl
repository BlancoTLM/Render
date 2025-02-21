#version 410

layout(location = 0) in vec2 in_position;
uniform float aspect_ratio;
uniform float offset;

void main()
{
    vec2 shaderPos = vec2(in_position.x / aspect_ratio + offset, in_position.y);
    gl_Position = vec4(shaderPos, 0., 1.);
}
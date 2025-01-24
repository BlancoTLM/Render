#version 410

layout(location = 0) in vec2 in_position;

void main()
{
    vec2 shaderPos = in_position;
    shaderPos += vec2(0.2, 0.2);
    gl_Position = vec4(shaderPos, 0., 1.);
}
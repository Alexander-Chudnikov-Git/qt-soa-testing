#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec4 texc;

void main(void)
{
    gl_Position = vec4(position, 0.0, 1.0);
    texc = vec4(texCoord, 0.0, 1.0);
}

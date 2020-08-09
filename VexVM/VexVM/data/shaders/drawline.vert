#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 color;

out vec4 lineColor;

void main()
{
    lineColor = vec4(color.r, color.g, color.b, 1.0f);
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
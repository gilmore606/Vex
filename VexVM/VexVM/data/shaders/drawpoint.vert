#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 color;
layout (location = 2) in float size;

out vec4 pointColor;

void main()
{
    pointColor = vec4(color.r, color.g, color.b, 1.0f);
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    gl_PointSize = size;
}
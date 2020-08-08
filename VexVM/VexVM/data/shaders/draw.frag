#version 330 core
in vec4 lineColor;
out vec4 FragColor;
uniform float brightness = 1.0f;

void main()
{
    FragColor = lineColor * brightness;
}
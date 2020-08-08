#version 330 core
out vec4 FragColor;
uniform float brightness = 1.0f;

void main()
{
    FragColor = vec4(0.8f, 0.9f, 1.0f, 1.0f) * brightness;
}
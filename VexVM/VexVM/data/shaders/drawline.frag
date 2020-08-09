#version 330 core
in vec4 lineColor;
out vec4 FragColor;
uniform float brightness = 1.0f;

void main()
{
    if (brightness <= 1.0f) {
        FragColor = lineColor * brightness;
    } else {
        float r = lineColor.r * brightness;
        float g = lineColor.g * brightness;
        float b = lineColor.b * brightness;
        float maxcolor = max(r, max(g, b));
        if (maxcolor > 1.0f) {
            float over = maxcolor - 1.0f;
            r = r + over;
            g = g + over;
            b = b + over;
        }
        FragColor = vec4(r, g, b, 1.0);
    }
}
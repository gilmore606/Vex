#version 330 core
in vec4 lineColor;
out vec4 FragColor;
uniform float brightness;

void main()
{
    if (brightness <= 1.0f) {
        FragColor = vec4(lineColor.r * brightness, lineColor.g * brightness, lineColor.b * brightness, 1.0f);
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

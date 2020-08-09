#version 330 core
in vec4 pointColor;
out vec4 FragColor;
uniform float brightness = 1.0f;

void main()
{
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    if (dot(circCoord, circCoord) > 1.0) {
        discard;
    } else {
        if (brightness <= 1.0f) {
            FragColor = pointColor * brightness;
        } else {
            float r = pointColor.r * brightness;
            float g = pointColor.g * brightness;
            float b = pointColor.b * brightness;
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
}
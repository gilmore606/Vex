#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D inTexture;
uniform float decay;

void main()
{
    float thresh = 0.007f;
    vec3 color = texture(inTexture, TexCoords).rgb * decay;
    FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    if (color.r > thresh) FragColor.r = color.r;
    if (color.g > thresh) FragColor.g = color.g;
    if (color.b > thresh) FragColor.b = color.b;
}

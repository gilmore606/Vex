#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D inTexture;

void main()
{
    vec3 color = texture(inTexture, TexCoords).rgb;
    FragColor = vec4(color * 0.95f, 1.0);
}

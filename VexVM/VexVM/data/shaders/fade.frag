#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D inTexture;
uniform float decay;

void main()
{
    vec3 color = texture(inTexture, TexCoords).rgb;
    FragColor = vec4(color * decay, 1.0);
}

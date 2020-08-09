#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTex;
uniform sampler2D glowTex;

void main()
{
    vec3 color = texture(screenTex, TexCoords).rgb;
    vec3 color2 = texture(glowTex, TexCoords).rgb;
    FragColor = vec4(color2 + color, 1.0);
}

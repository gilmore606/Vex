#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTex;
uniform sampler2D glowTex;
uniform sampler2D trailTex;

void main()
{
    vec3 screen = texture(screenTex, TexCoords).rgb;
    vec3 glow = texture(glowTex, TexCoords).rgb;
    vec3 trail = texture(trailTex, TexCoords).rgb;
    float r = min(1.0, max(screen.r, max(glow.r, trail.r)));
    float g = min(1.0, max(screen.g, max(glow.g, trail.g)));
    float b = min(1.0, max(screen.b, max(glow.b, trail.b)));

    FragColor = vec4(r,g,b,1.0);
}

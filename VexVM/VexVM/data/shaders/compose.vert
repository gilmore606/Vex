#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform sampler2D screenTex;
uniform sampler2D glowTex;
uniform float xscale;
uniform float yscale;

out vec2 TexCoords;

void main()
{
    TexCoords = vec2(aTexCoords.x / xscale, aTexCoords.y / yscale);
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

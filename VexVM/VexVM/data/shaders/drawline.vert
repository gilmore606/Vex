#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 color;

out vec4 lineColor;
uniform float time;
uniform float spread;
uniform float stability;
uniform float sustain;
uniform float drop;

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

void main()
{
    vec2 seed1 = vec2(time + aPos.x, time + aPos.y);
    vec2 seed2 = vec2(time + aPos.y, time + aPos.x);
    float deviance = spread;
    if (rand(seed2 + 5.5) > stability) {
        deviance = 0.006f;
    }
    float xj = rand(seed1) * deviance;
    float yj = rand(seed2) * deviance;

    float flicker = 1.0f;
    if (rand(seed1 + 17.3) > sustain) {
        flicker = drop;
    }
    lineColor = vec4(color.r, color.g, color.b, 1.0f) * flicker;
    gl_Position = vec4(aPos.x + xj, aPos.y + yj, 0.0, 1.0);
}
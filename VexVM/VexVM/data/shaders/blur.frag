#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D texture;

void main()
{
    float offset = (2.2 / 2000.0);
    float sample = 0.0f;
    vec4 c = vec4(0);
    vec2 s = vec2(0);
    s.x = uv.x;
    s.y = uv.y - offset;
    c += 5.0 * texture2D(texture, s);
    s.x = uv.x - offset;
    s.y = uv.y;
    c += 5.0 * texture2D(texture, s);
    c += 6.0 * texture2D(texture, uv);
    s.x = uv.x;
    s.y = uv.y + offset;
    c += 5.0 * texture2D(texture, s);
    s.x = uv.x + offset;
    s.y = uv.y;
    c += 5.0 * texture2D(texture, s);
    FragColor = c / 26.0;
}
#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D texture;
uniform float resolution;
uniform float radius;
uniform float dirx;
uniform float diry;

void main()
{
    vec4 sum = vec4(0.0);
	vec2 tc = uv;
	
	//the amount to blur, i.e. how far off center to sample from 
	float blur = radius/resolution; 
	float hstep = dirx;
	float vstep = diry;
    
	sum += texture2D(texture, vec2(tc.x - 4.0*blur*hstep, tc.y - 4.0*blur*vstep)) * 0.0162162162;
	sum += texture2D(texture, vec2(tc.x - 3.0*blur*hstep, tc.y - 3.0*blur*vstep)) * 0.0540540541;
	sum += texture2D(texture, vec2(tc.x - 2.0*blur*hstep, tc.y - 2.0*blur*vstep)) * 0.1216216216;
	sum += texture2D(texture, vec2(tc.x - 1.0*blur*hstep, tc.y - 1.0*blur*vstep)) * 0.1945945946;
	
	sum += texture2D(texture, vec2(tc.x, tc.y)) * 0.2270270270;
	
	sum += texture2D(texture, vec2(tc.x + 1.0*blur*hstep, tc.y + 1.0*blur*vstep)) * 0.1945945946;
	sum += texture2D(texture, vec2(tc.x + 2.0*blur*hstep, tc.y + 2.0*blur*vstep)) * 0.1216216216;
	sum += texture2D(texture, vec2(tc.x + 3.0*blur*hstep, tc.y + 3.0*blur*vstep)) * 0.0540540541;
	sum += texture2D(texture, vec2(tc.x + 4.0*blur*hstep, tc.y + 4.0*blur*vstep)) * 0.0162162162;

	FragColor = vec4(sum.rgb, 1.0);
}
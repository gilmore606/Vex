#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTex;
uniform sampler2D glowTex;
uniform sampler2D trailTex;



// getCenterDistance
// Gets Distance between points in the coord space and the center of that space
// @param	vec2	coord		coordinate space to caculate distance to center
// @return	float				distance from center of plane (0 in center, -1 and 1 at edges)
float getCenterDistance(vec2 coord)
{
    return distance(coord, vec2(0.5)) * 2.0; //return difference between point on screen and the center with -1 and 1 at either edge
}


// bulgeCoords
// Bulges Coordinates to perform a bulging lens effect
// @param	vec2	coord		coordinates to distort
// @param	vec2	sourceCoord	source coordinate space to caculate distance to center
// @param 	float	bulgeAmount	amount to bulge coordindates
// @return	vec2				distorted sample coordinates
vec2 bulgeCoords(vec2 coord, vec2 sourceCoord, float bulgeAmount)
{
    float centerDist = getCenterDistance(sourceCoord);
    
    coord.xy -= vec2(0.5); //reposition so scaling performed from center of image
    
    coord.xy *= 1.0 + centerDist * bulgeAmount; //scale up coordinates the further from center they are
    coord.xy *= 1.0 - bulgeAmount; //scale down oversampling to reduce tiling
    
    coord.xy += vec2(0.5); //restore position to center of view
    
    return coord;
}


// sampleRGBVignette
// Samples texture pixels with RGB color abberation shift towards the edges of the view, simulating lens color abberation
// @param	sampler2D	source		source texture to sample
// @param	vec2		coord		coordinates to distort
// @param	vec2		sourceCoord	source coordinate space to caculate distance to center
// @param 	float		amount		amount to offset Green and Blue channels (B offset twice as far from R)
// @param 	float		power		bias of color abberation just towards edges of view (>= 1)
// @return	vec4					resultant sample color with color RGB abberation
vec4 sampleRGBVignette(sampler2D source, vec2 coord, vec2 sourceCoord, float amount, float power)
{
    float centerDist = getCenterDistance(sourceCoord);
    centerDist = pow(centerDist, power); //bias distance from center to ramp up steeper towards edges
    
    vec2 sampleCoord = coord;
    vec4 outputColor = texture(source, fract(sampleCoord)); //get default sample image (for R)
    
    sampleCoord = bulgeCoords(coord, sourceCoord, amount * centerDist); //bulge sample coordinates by amount, multiply by center distance to reduce effect in center
    outputColor.g = texture(source, fract(sampleCoord)).g; //sample Green amount by G color abberation
    
    sampleCoord = bulgeCoords(coord, sourceCoord, amount * 2.0 * centerDist); //bulge sample coordinates by double amount for Blue (twice as far from R as G)
    outputColor.b = texture(source, fract(sampleCoord)).b; //sample Blue amount by B color abberation
    
    return outputColor;
}

// applyVignette
// Applys darkened vignette to edges of view
// @param	vec4		color		original color value
// @param	vec2		coord		source coordinate space to caculate distance to center
// @param 	float		amount		amount to darken gaps vignette edges
// @param 	float		scale		scale of vignette outer ellipse (1 = darkest at ellipse incribing view)
// @param 	float		power		bias vignette fade towards edges of ellipse (>= 1)
// @return	vec4					resultant color with vignette applied
vec4 applyVignette(vec4 color, vec2 sourceCoord, float amount, float scale, float power)
{
    float centerDist = getCenterDistance(sourceCoord);
    
    float darkenAmount = centerDist / scale; //get amount to darken current fragment by scaled distance from center
    
    darkenAmount = pow(darkenAmount, power); //bias darkenAmount towards edges of distance
    
    darkenAmount = min(1.0, darkenAmount); //clamp maximum darkenAmount to 1 so amount param can lighten outer regions of vignette 
    
    color.rgb -= darkenAmount * amount; //darken rgb colors by given vignette amount
    
    return color;
}




void main()
{
    vec2 sourceCoord = TexCoords;
    vec2 sampleCoord = sourceCoord;
    vec4 screenColor = sampleRGBVignette(screenTex, sampleCoord, sourceCoord, 0.03, 1.3);
    vec3 abb = applyVignette(screenColor, sourceCoord, 1.4, 2.0, 2.7).rgb;
    vec3 source = texture(screenTex, TexCoords).rgb;
    vec3 screen = source + vec3(abb.r * 0.5, abb.g * 0.5, abb.b * 0.5);
    vec3 glow = texture(glowTex, TexCoords).rgb;
    vec3 trail = texture(trailTex, TexCoords).rgb;
    float r = min(1.0, max(screen.r, max(glow.r, trail.r)));
    float g = min(1.0, max(screen.g, max(glow.g, trail.g)));
    float b = min(1.0, max(screen.b, max(glow.b, trail.b)));

    FragColor = vec4(r,g,b,1.0);
}

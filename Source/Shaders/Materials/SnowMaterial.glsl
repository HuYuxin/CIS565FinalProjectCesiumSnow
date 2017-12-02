uniform sampler2D image;


 /* Author: Stefan Gustavson ITN-LiTH (stegu@itn.liu.se) 2004-12-05
 * You may use, modify and redistribute this code free of charge,
 * provided that my name and this notice appears intact.
 *  The fade and the noise function are meant for a GLSL shader and 
 *  were writtend by Stefan Gustavsson
 */

#define ONE 0.00390625
#define ONEHALF 0.001953125


/*
 * The interpolation function. This could be a 1D texture lookup
 * to get some more speed, but it's not the main part of the algorithm.
 */
float fade(float t) {
  // return t*t*(3.0-2.0*t); // Old fade, yields discontinuous second derivative
  return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade, yields C2-continuous noise
}
// Scale is the extent of the square over which the noise repeats
#define SCALE 1000
//ImageSize is the size of the image used for Random noise
#define ImageSize 256
// Fraction Noise is the fraction of the noise to use to modify the
// blend coefficient.
#define FractionNoise 0.8
#define persistance 0.5
#define orders       3;
/*
 * 2D classic Perlin noise. Fast, but less useful than 3D noise.
 */
float noise(vec2 P)
{
  vec2 Pi = ONE*floor(P)+ONEHALF; // Integer part, scaled and offset for texture lookup
  vec2 Pf = fract(P);             // Fractional part for interpolation

  // Noise contribution from lower left corner
  vec2 grad00 = texture2D(image, Pi).rg * 4.0 - 1.0;
  float n00 = dot(grad00, Pf);

  // Noise contribution from lower right corner
  vec2 grad10 = texture2D(image, Pi + vec2(ONE, 0.0)).rg * 4.0 - 1.0;
  float n10 = dot(grad10, Pf - vec2(1.0, 0.0));

  // Noise contribution from upper left corner
  vec2 grad01 = texture2D(image, Pi + vec2(0.0, ONE)).rg * 4.0 - 1.0;
  float n01 = dot(grad01, Pf - vec2(0.0, 1.0));

  // Noise contribution from upper right corner
  vec2 grad11 = texture2D(image, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float n11 = dot(grad11, Pf - vec2(1.0, 1.0));

  // Blend contributions along x
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(Pf.x));

  // Blend contributions along y
  float n_xy = mix(n_x.x, n_x.y, fade(Pf.y));

  // We're done, return the final noise value.
  return n_xy;
}
// returns the x - n * scale where n is the 
// largest integer that keeps x positive
// scale should be positive. 
// 
// takes the fractional part of pos ( pos - floor(pos))
// which is guaranteed to be between 0 and 1 and scale it
// by scale
vec2  ScaleCoordinate(vec2 pos, float scale)
{
         return fract(pos) * scale;
}
      
czm_material czm_getMaterial(czm_materialInput materialInput)
{
    czm_material material = czm_getDefaultMaterial(materialInput);
	
	//Added by Yuxin For Test
	//vec3 normal = normalize(materialInput.normalEC);
	float snowSlope = materialInput.slope;
	material.alpha = snowSlope;
	// eye coordinate
	vec2  posCoord = materialInput.positionToEyeEC.xz;
	posCoord /= float(SCALE); 
        vec2  posScaled = ScaleCoordinate(posCoord, float(ImageSize) );
	float noiseval = noise(posScaled);
        material.alpha += FractionNoise * noiseval;
//	vec3 color =texture2D(image,  materialInput.str.xy).rgb;
	material.diffuse = vec3(0.8, 0.8, 0.9);
	//material.diffuse = materialInput.normalEC;
    //material.alpha = 1.0;
	
    return material;
}

uniform sampler2D level1normalMap;
uniform sampler2D whiteNoise;
uniform sampler2D level2normalMap;
uniform sampler2D level3normalMap;
uniform sampler2D level4normalMap;
uniform sampler2D level5normalMap;
uniform float accumulationStartTime;
uniform float accumulationEndTime;

// Scale is the extent of the square over which the noise repeats
float SCALE = 100000.;
//ImageSize is the size of the image used for Random noise
const float ImageSize = 256.;
// Fraction Noise is the fraction of the noise to use to modify the
// blend coefficient.
const float FractionNoise = 0.3;
const float persistance =  0.6;
// orders are the number of orders to sum
const float  orders = 3.;
// add snow only when the level is above this value
const float threshold = 0.1;
const float highthreshold = 0.9;

// the base noise functions came from Ian McEwan and Stephan Gustavson.
// Using this code to generate noise was from Yuxin Hu, Anton Khabbaz, and YaoYi
//
// Description : Array and textureless GLSL 2D/3D/4D simplex 
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
// 

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  }

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
/*
 * 2D classic Perlin noise. Fast, but less useful than 3D noise.
 */
float noise(vec2 P)
{
  vec2 Pi = ONE*floor(P)+ONEHALF; // Integer part, scaled and offset for texture lookup
  vec2 Pf = fract(P);             // Fractional part for interpolation

  // Noise contribution from lower left corner
  vec2 grad00 = texture2D(whiteNoise, Pi).rg * 4.0 - 1.0;
  float n00 = dot(grad00, Pf);

  // Noise contribution from lower right corner
  vec2 grad10 = texture2D(whiteNoise, Pi + vec2(ONE, 0.0)).rg * 4.0 - 1.0;
  float n10 = dot(grad10, Pf - vec2(1.0, 0.0));

  // Noise contribution from upper left corner
  vec2 grad01 = texture2D(whiteNoise, Pi + vec2(0.0, ONE)).rg * 4.0 - 1.0;
  float n01 = dot(grad01, Pf - vec2(0.0, 1.0));

  // Noise contribution from upper right corner
  vec2 grad11 = texture2D(whiteNoise, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float n11 = dot(grad11, Pf - vec2(1.0, 1.0));

  // Blend contributions along x
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(Pf.x));

  // Blend contributions along y
  float n_xy = mix(n_x.x, n_x.y, fade(Pf.y));

  // We're done, return the final noise value.
  return n_xy;
}
/*
 * 3D classic noise. Slower, but a lot more useful than 2D noise.
 */
float noise(vec3 P)
{
  vec3 Pi = ONE*floor(P)+ONEHALF; // Integer part, scaled so +1 moves one texel
                                  // and offset 1/2 texel to sample texel centers
  vec3 Pf = fract(P);     // Fractional part for interpolation

  // Noise contributions from (x=0, y=0), z=0 and z=1
  float perm00 = texture2D(whiteNoise, Pi.xy).a ;
  vec3  grad000 = texture2D(whiteNoise, vec2(perm00, Pi.z)).rgb * 4.0 - 1.0;
  float n000 = dot(grad000, Pf);
  vec3  grad001 = texture2D(whiteNoise, vec2(perm00, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n001 = dot(grad001, Pf - vec3(0.0, 0.0, 1.0));

  // Noise contributions from (x=0, y=1), z=0 and z=1
  float perm01 = texture2D(whiteNoise, Pi.xy + vec2(0.0, ONE)).a ;
  vec3  grad010 = texture2D(whiteNoise, vec2(perm01, Pi.z)).rgb * 4.0 - 1.0;
  float n010 = dot(grad010, Pf - vec3(0.0, 1.0, 0.0));
  vec3  grad011 = texture2D(whiteNoise, vec2(perm01, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n011 = dot(grad011, Pf - vec3(0.0, 1.0, 1.0));

  // Noise contributions from (x=1, y=0), z=0 and z=1
  float perm10 = texture2D(whiteNoise, Pi.xy + vec2(ONE, 0.0)).a ;
  vec3  grad100 = texture2D(whiteNoise, vec2(perm10, Pi.z)).rgb * 4.0 - 1.0;
  float n100 = dot(grad100, Pf - vec3(1.0, 0.0, 0.0));
  vec3  grad101 = texture2D(whiteNoise, vec2(perm10, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n101 = dot(grad101, Pf - vec3(1.0, 0.0, 1.0));

  // Noise contributions from (x=1, y=1), z=0 and z=1
  float perm11 = texture2D(whiteNoise, Pi.xy + vec2(ONE, ONE)).a ;
  vec3  grad110 = texture2D(whiteNoise, vec2(perm11, Pi.z)).rgb * 4.0 - 1.0;
  float n110 = dot(grad110, Pf - vec3(1.0, 1.0, 0.0));
  vec3  grad111 = texture2D(whiteNoise, vec2(perm11, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n111 = dot(grad111, Pf - vec3(1.0, 1.0, 1.0));

  // Blend contributions along x
  vec4 n_x = mix(vec4(n000, n001, n010, n011),
                 vec4(n100, n101, n110, n111), fade(Pf.x));

  // Blend contributions along y
  vec2 n_xy = mix(n_x.xy, n_x.zw, fade(Pf.y));

  // Blend contributions along z
  float n_xyz = mix(n_xy.x, n_xy.y, fade(Pf.z));

  // We're done, return the final noise value.
  return n_xyz;
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
vec3  ScaleCoordinate(vec3 pos, float scale)
{
         return fract(pos) * scale;
}

czm_material czm_getMaterial(czm_materialInput materialInput)
{
    czm_material material = czm_getDefaultMaterial(materialInput);

	//Added by Yuxin For Test
	float snowSlope = materialInput.slope;
	material.alpha = snowSlope;
    material.diffuse = vec3(0.8, 0.8, 0.9);
    material.shininess = 200.0;
    float snowAccumulation = 0.0;

    float interval = accumulationEndTime - accumulationStartTime;
    float lapse = czm_frameNumber - accumulationStartTime;
    if(lapse < 1.5 * interval){
        snowAccumulation = lapse / interval;
    }else{
        snowAccumulation = 1.5;
    }

    vec3 normalMapNormal = vec3(0.0, 0.0, 1.0);
    if(snowAccumulation < 0.3){
        normalMapNormal = texture2D(level1normalMap, materialInput.st).rgb;
    }else if(snowAccumulation < 0.7){
        normalMapNormal = texture2D(level2normalMap, materialInput.st).rgb;
    }else if(snowAccumulation < 1.0){
        normalMapNormal = texture2D(level3normalMap, materialInput.st).rgb;
    }else if(snowAccumulation < 1.3){
        normalMapNormal = texture2D(level4normalMap, materialInput.st).rgb;
    }else{
        normalMapNormal = texture2D(level5normalMap, materialInput.st).rgb;
    }

    mat3 tangentToEye = materialInput.tangentToEyeMatrix;
    material.normal  = tangentToEye * normalMapNormal;

    //material.normal = materialInput.normalEC;
    material.specular = 0.9;
    // now this is really the model coordinate position
    //vec2  posCoord = materialInput.positionToEyeEC.xz;
    vec3 posCoord = materialInput.positionToEyeEC.xzy;
    posCoord /= czm_currentFrustum.y;
    float noiseval = 0.;
    for(float idx = 0.; idx < orders; ++idx)
    {
	    // take multiples of the position coordinate
	    // to sample different regions of the texture
	    vec3 posScaled = idx * posCoord;
	    float amplitude = pow (persistance, idx);
	    posScaled = amplitude * ScaleCoordinate(posCoord, ImageSize);
	    noiseval += snoise(posScaled);
    }
    material.alpha += FractionNoise * noiseval;
    //material.alpha *= snowAccumulation;
    if ( material.alpha < threshold) {
	    material.alpha = 0.;
	}
	else if ( material.alpha > highthreshold) {
	     material.alpha = 1.;
    }
    return material;
}






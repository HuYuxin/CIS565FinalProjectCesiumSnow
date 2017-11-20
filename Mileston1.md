# Cesium Snow Rendering Milestone1

## Prepared by Anton Khabbaz, Yaoyi Bai, Yuxin Hu

### Overview

In this milestone we have achieved a simple snow rendering on terrain by applying a material. We have implemented the snow falling particles in shadertoy using ray marching.

### Features Implemented

**Snow Material on Terrain**

* We made use of material feature implemented in Cesium. We created a new snow material, and set the diffuse color of the material to be vec3(0.8,0.8,0.9), then set the alpha value to be dot product between surface normal in world space and y-axis of world. The idea comes from the paper Real-time rendering of accumulated snow by Ohlsson, P. and Seipel, S. In their paper, they have presented a method to determine the color of vertices by linear interpolating between vertex color and snow color:

![](/image/FullSnowEquation.PNG)
<p>Figure 1. color of accumulated snow by Ohlsson, P. and Seipel</p>

Where the fp indicates the amount of snow that could cover this vertex. fp is determined by:

![](/image/SnowAccumulatePredictionFuntion.PNG)
<p>Figure 2. snow accumulation prediction function by Ohlsson, P. and Seipel</p>

fe stands for exposure component, and finc stands for snow contribution due to inclination. In our first milestone we have focused on finc term. The more inclined the terrain is, the less snow accumulated on. This can be shown in the image below:

![](/image/SnowAccumulationFunctionInclined.PNG)
<p>Figure 3. Inclined surface by Ohlsson, P. and Seipel</p>

The cos angle is a good representation of how steep a vertex is. The final form of finc term is:

![](/image/SnowAccumulationFuncInclinedEquation.PNG)
<p>Figure 4. Inclined Term by Ohlsson, P. and Seipel</p>

Inside czm_materialInput object, it stores a value slope. It is the absolute value of dot product between up vector in world and normal of the vertex, both in model space, which is the cos term in finc. We set the alpha value of material to be value of slope. Inside GlobeFS.glsl it is linearly blending terrain original color and snow material diffuse color by amount of alpha. In our case it would be 

**finalColor = (1-slope) * originalColor + slope * snowColor** 

Here is a result of snow material:
![Grand Canyon Without Snow](/image/GrandCanyonNoSnow17Nov.PNG)  |  ![Grand Canyon With Snow](/image/GrandCanyonWithSnow.PNG)

**Snow Falling Particles**
* Snow Falling on Screen Space
The falling snow are rendered based on the combination of:
1) Assigned fall speed;
2) Sin function based on system time to create wind effect;
3) Gray noise image with ray intersection point and wind direction to determine the alpha channel of snow in that pixel;

After the ray marching of every pixel, calculate the alpha channel color based on the ideas given above, and linearly interpolate white color and the background color based on the value of the alpha channel.

Then we can play with the parameters to change the snow effect.
![](/image/snow1.gif)
<p>Heavy snow</p>

![](/image/snow2.gif)
<p>Light snow</p>

![](/image/snow3.gif)
<p>Accelerated heavy snow</p>

* Ray Marching (coming soon)
In fragment shader, implementing ray marching based on the gbuffer to determine what color and terrain geometry to “color” on screen. Currently, we only have a sphere at the center of the scene.

Ultimately, the algorithm should be:
```
if (ray intersects with some geometry before t reaches maximum)
{
	calculate the intersection point
	calculate the length between the origin and intersection point
	color the pixel based on the terrain material color (we have a sphere in the scene right now)
}else
{
	set the distance between the intersection point and ray origin to be tmax
	color the pixel based on the material of sky
}
```


### Plans for future milestones

* Milestone 2: Integrate the snow particle rendering in Cesium. Normal map on the snow and perlin noise to get the snow to look more realistic. 

* Milestone 3: Make the snow particle render modifiable through the website.

* Milestone 4: ???

### Bloopers
* In czm_materialInput, there is a normalEC indicates the normal of vertex in eye space. When camera angle changes, the normalEC value changes, too.

![](/image/TerrainNormalEyeSpace.gif)

Initially we calculate the finc term by calculating dot product of normalEC and vec3(0.0, 1.0, 0.0), which we assume is the normal of the world space. This was not correct because the two normals are not in the same frame of reference. When rendering, it looks like snow is on the terrain, however when camera rotates, snow dissappears. 

![](/image/SnowRenderBlooper.gif)

### References

1. Ohlsson, P. and Seipel, S., 2004, November. Real-time rendering of accumulated snow. In The Annual SIGRAD Conference. Special Theme-Environmental Visualization (No. 013, pp. 25-31). Linköping University Electronic Press.

2. CIS 565 Fall 2015 https://github.com/CIS565-Fall-2015/Project5-GLSL-Ray-Marcher#resources

3. Shadertoy snow https://www.shadertoy.com/view/ltfGzn







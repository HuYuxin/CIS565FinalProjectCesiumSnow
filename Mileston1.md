# Cesium Snow Rendering Milestone1

## Prepared by Anton Khabbaz, Yaoyi Bai, Yuxin Hu

**Overview**

In this milestone we have achieved a simple snow rendering on terrain by applying a material. We have implemented the snow falling particles in shadertoy using ray marching.

**Features Implemented**

Snow Material on Terrain

* We made use of material feature implemented in Cesium. We created a new snow material, and set the diffuse color of the material to be vec3(0.8,0.8,0.9), then set the alpha value to be dot product between surface normal in world space and y-axis of world. The idea comes from the paper Real-time rendering of accumulated snow by Ohlsson, P. and Seipel, S. In their paper, they have presented a method to determine the color of vertices by linear interpolating between vertex color and snow color:

![](/image/FullSnowEquation.PNG)
<p align="center"><b>color of accumulated snow by Ohlsson, P. and Seipel</b></p>

Where the fp indicates the amount of snow that could cover this vertex. fp is determined by:

![](/image/SnowAccumulatePredictionFuntion.PNG)
<p align="center"><b>snow accumulation prediction function by Ohlsson, P. and Seipel</b></p>

fe stands for exposure component, and finc stands for snow contribution due to inclination. In our first milestone we have focused on finc term. The more inclined the terrain is, the less snow accumulated on. This can be shown in the image below:

![](/image/SnowAccumulationFunctionInclined.PNG)
<p align="center"><b>Inclined surface by Ohlsson, P. and Seipel</b></p>

The cos angle is a good representation of how steep a vertex is. The final form of finc term is:

![](/image/SnowAccumulationFuncInclinedEquation.PNG)
<p align="center"><b>Inclined Term by Ohlsson, P. and Seipel</b></p>

Inside czm_materialInput object, it stores a value slope. It is the absolute value of dot product between up vector in world and normal of the vertex, both in model space, which is the cos term in finc. We set the alpha value of material to be value of slope. Inside GlobeFS.glsl it is linearly blending terrain original color and snow material diffuse color by amount of alpha. In our case it would be 

**finalColor = (1-slope)*originalColor + slope*snowColor 

Here is a result of snow material:
| No Anti-aliasing             |  With Anti-aliasing |
:-------------------------:|:-------------------------:
![Grand Canyon Without Snow](/image/GrandCanyonNoSnow17Nov.PNG)  |  ![Grand Canyon With Snow](/image/GrandCanyonWithSnow.PNG)

**Goals**

Implementation Based on Particle System in Cesium
* Snow falling effect
* Snow gathering on the terrain  
* Snow “melting” (snow will not infinitely gather on the terrain)
* Real-time snow effects allowing the user to modify the snow fall over the current view

**Weekly goals**
* Week1: Get the basic framework working in our hands.  Have snow on the ground. Get the particles falling with gravity and wind resistance.
* Week2: Make the snow particle system modifiable through the website. Normal map on the snow and perlin noise to get the snow to look more realistic. 
* Week3: Integrate the systems into a cohesive unit. Add mechanism to determine which surfaces will accumulate snow.
* Week4: Adding wind gusts. Take into account melting by linear interpolation between the snow color and the terrain color.  Demonstrate rendering in smaller scale scenes such as urban environments.

**References**

Ohlsson, P. and Seipel, S., 2004, November. Real-time rendering of accumulated snow. In The Annual SIGRAD Conference. Special Theme-Environmental Visualization (No. 013, pp. 25-31). Linköping University Electronic Press.



Snow Rendering in Cesium
========================

**University of Pennsylvania, CIS 565: GPU Programming and Architecture, Final Project**

**Team Members**

[Yaoyi Bai](https://github.com/VElysianP)

[Anton Khabbaz](https://github.com/akhabbaz)

[Yuxin Hu](www.huyuxin.net)

### Demo
![](/image/Demo7.gif)
<p>Snow Rendering Over Grand Canyon</p>

![](/image/Demo5.gif)
<p>Snow Rendering Over Bay Area</p>

### Debug Views

Normal Map                             |  Surface Normal with Normal Map
:-------------------------------------:|:---------------------------------------------------:
Original Terrain Surface Normal        |  ![](/image/FinalDebugNormalEC.PNG)
![](/image/snowNormalMapLevel1.jpg)    |  ![](/image/FinalDebugNormalMap1EC.PNG)
![](/image/snowNormalMapLevel2.jpg)    |  ![](/image/FinalDebugNormalMap2EC.PNG)
![](/image/snowNormalMapLevel3.jpg)    |  ![](/image/FinalDebugNormalMap3EC.PNG)
![](/image/snowNormalMapLevel4.jpg)    |  ![](/image/FinalDebugNormalMap4EC.PNG)
![](/image/snowNormalMapLevel5.jpg)    |  ![](/image/FinalDebugNormalMap5EC.PNG)


Slope Without Perlin Noise             |  Slope With Perlin Noise
:-------------------------------------:|:---------------------------------------------------:
![](/image/FinalDebugSlopeNoNoise.PNG) |  ![](/image/FinalDebugSlopWithNoise.PNG)


### Progress
* [Project Pitch](FinalProjectPitch.md)
* [Milestone 1](Mileston1.md)
* [Milestone 2](Milestone2.md)
* [Milestone 3](Milestone3.md)


### Overview of Technique
The goal of this project is to add snow rendering feature in Cesium. We approached the problem from two parts. For snow falling particles, we referred to the ray marching sampling method in Shadertoy(https://www.shadertoy.com/view/ltfGzn). For snow accumulation on ground, we applied a Cesium snow material over the terrain. The idea is taken from paper Real-time Rendering of Accumulated Snow(http://www.ep.liu.se/ecp/013/007/ecp01307.pdf). The snow material has a fixed diffuse color, and a fixed specular term. The alpha value of snow material is depending on the slope value of the terrain fragment. We added Perlin noise to slope and normal maps to make the snow looks more realistic. We finally we made the slope and normal maps as a function of time to animate the snow accumulation process.


### Performance Analysis (Coming Soon)


### Files Changed (Coming Soon)


### Credits

* [Cesium Official Website, tutorial and Code Samples](http://cesiumjs.org/)
* [Cesium Source Code](https://github.com/AnalyticalGraphicsInc/cesium)
* [Real-time Rendering of Accumulated Snow](http://www.ep.liu.se/ecp/013/007/ecp01307.pdf)
* [Shadertoy Snow Rendering](https://www.shadertoy.com/view/ltfGzn)
* [glsl Noise Algorithms](https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83)

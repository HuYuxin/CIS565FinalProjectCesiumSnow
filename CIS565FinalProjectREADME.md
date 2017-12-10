Snow Rendering in Cesium
========================

**University of Pennsylvania, CIS 565: GPU Programming and Architecture, Final Project**

**Team Members**

[Yaoyi Bai](https://github.com/VElysianP)

[Anton Khabbaz](https://github.com/akhabbaz)

[Yuxin Hu](https://www.huyuxin.net)

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


### Files Changed

**Merged branch *Globe Materials* with branch *post-processing-1.0* into our master branch**

#### 1. Snow Accumulation 

- Added SnowMaterial.glsl in "..\Source\Shaders\Materials" to apply snow color over terrain. Inside includes perlin noise generation that will be added to alpha value of the material, which is based on the terrain slope.

- Revised GlobeFS.glsl in "..\Source\Shaders\". Added variables in main() : normalWithNoise, shiness, and specular that will be assigned values in SnowMaterial.glsl. Added a boolean variable isOcean which will be assigned value if water mask is applied. This will ensure that material will only be added over terrain that is not Ocean. Modified ifdef APPLY_MATERIAL section to read extra values added in material. Modified #ifdef ENABLE_VERTEX_LIGHTING. We are checking if shiness read from material is bigger than 0, we used the modified normal read from material for shading and added a specular component in shading. 

- Revised Material.js in "..\Source\Scene", added a new material SnowMaterial.

#### 2. Snow Falling 

- Added *SnowFalling.glsl* in "..\Source\Shaders\PostProcessFilters";

- Revised *PostProcessLibrary.js* in "..\Source\Scene", a new post process stage called *createSnowFallingStage* added and a new fragment shader called *SnowFalling* linked to this post process stage;

- Revised *PostProcessScene.js* in "..\Source\Scene", a new stage called *snowFalling* added;

- Revised *Post Processing.html* in "..\Apps\Sandcastle\gallery", a new view model linked snow falling post process stage added to create snow falling effect based on the *depthTexture* in post process stage. 

#### 3. Demo HTML
- We added a demo html page to show our snow rendering feature, under Apps/Sandcastle/gallery/, named "SnowRendering.html".

#### 4. Texture Images Added
- Source\Assets\Textures\grayNoiseM.png. This texture is used to for snow flake sampling.

- Under Apps\Sandcastle\images\, snowNormalMapLevel1.jpg, snowNormalMapLevel2.jpg, snowNormalMapLevel3.jpg, snowNormalMapLevel4.jpg, snowNormalMapLevel5.jpg. These normal maps are used for bumpy snow material over the terrain.

- Under Apps\Sandcastle\images\RandomNoise256.png. This image is used for perlin noise generation.

### Credits

* [Cesium Official Website, tutorial and Code Samples](http://cesiumjs.org/)
* [Cesium Source Code](https://github.com/AnalyticalGraphicsInc/cesium)
* [Real-time Rendering of Accumulated Snow](http://www.ep.liu.se/ecp/013/007/ecp01307.pdf)
* [Shadertoy Snow Rendering](https://www.shadertoy.com/view/ltfGzn)
* [glsl Noise Algorithms](https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83)

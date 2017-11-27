# Cesium Snow Rendering Milestone2

## Prepared by Anton Khabbaz, Yaoyi Bai, Yuxin Hu

### Overview

In this milestone we added noise to terrain surface normal to get a more realistice snow look. We also tried to implement the snow falling effect in Cesium as a post-processing shader layer.

### Features Implemented

**Adding Noise to Surface normal**

* We added noise to terrain surface normal to get a more realistic accumulated snow look, as described by Ohlsson, P. and Seipel, S](http://www.ep.liu.se/ecp/013/007/ecp01307.pdf)  In their paper, they have proposed a method to generate Perlin noise to normal vector by sampling 3 octaves. We simplified the noise generation process by reading a pixel color from a colored perlin noise texture.

![](/image/perlin_colors.png)
<p>Colored Perlin Noise From Online(http://aave.phatcode.net/infiniverse/?p=articles-creating_nebulae.php)</p>

The original terrain surface normal in eye space is:

![](/image/NormalWithoutNoise.PNG)
<p>Normal without noise in eye space</p>

After we added noise to surface normal

![](/image/NormalWithNoise4.PNG)
<p>Normal with noise in eye space</p>

Here is a comparison of snow with and without noise added to surface normal:
![](/image/SnowWithouNormalNoise.PNG)  |  ![](/image/SnowWithNormalNoise3.PNG)

**Snow Falling Particles As Post Processing Layer**

* We tried to integrate the snow falling particles we implemented in shader toy in our last milestone(https://github.com/HuYuxin/CIS565FinalProjectCesiumSnow/blob/YuxinBranch/Mileston1.md) as a post processing layer to our snow accumulated rendering. As a simple test we implemented the ray marching and snow rendering inside snow material. To test we temporarily turned snow flake color from white to red. It may look weird that you only see red color mountain instead of red color snow flake for now, but we will bring the snowflake to screen soon :)

![](/image/mile2_1.jpg)

![](/image/mile2_2.jpg)


### Plans for future milestones

* Milestone 3: Integrate the snow particle rendering in Cesium. Make the snow particle render modifiable through the website.

* Milestone 4: ???

### References

1. Ohlsson, P. and Seipel, S., 2004, November. Real-time rendering of accumulated snow. In The Annual SIGRAD Conference. Special Theme-Environmental Visualization (No. 013, pp. 25-31). Linköping University Electronic Press.

2. Perlin Noise Texture
http://aave.phatcode.net/infiniverse/?p=articles-creating_nebulae.php
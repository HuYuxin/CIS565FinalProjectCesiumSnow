# Cesium Snow Rendering Proposal

## Prepared by Anton Khabbaz, Yaoyi Bai, Yuxin Hu

**Overview**

Currently in Cesium has great Terrain system that is capable of displaying accurate shapes of mountains and glaciers, like Mount Everest and Greenland shown below. 

![](/image/CesiumMountEverest.jpg)

![](/image/Greenland.jpg)

Cesium displays high or steep terrains with perspective, allowing the user to physically see mountain heights and shadows.  

What would be very nice to add is dynamic information that could modify the terrain to account for changes in weather.  Currently, snow is added as a texture, which has limitations: the texture modifies a preexisting scene to make it snow covered, and offers no depth information. Therefore, implementing the particle system to add in real-time snow on the terrain would make the system more realistic. Additionally, enabling particle to create snow particles makes it possible to implement falling snow. This can also become a stepping stone to implementing other weather effects like rain and hail.  Our project intends to upgrade the current terrain system in Cesium to account for seasonal climate changes and changing weather due to snow in real-time.  Here, we focus on rendering snow with the idea of creating a framework to incorporate in the future other weather-related changes in real time.

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
* Snow Falling
1. Tan, J. and Fan, X., 2011. Particle system based snow simulating in real time. Procedia Environmental Sciences, 10, pp.1244-1249.
2. Saltvik, I., Elster, A.C. and Nagel, H.R., 2006, June. Parallel methods for real-time visualization of snow. In International Workshop on Applied Parallel Computing (pp. 218-227). Springer, Berlin, Heidelberg.

* Snow Accumulation 
1. Ohlsson, P. and Seipel, S., 2004, November. Real-time rendering of accumulated snow. In The Annual SIGRAD Conference. Special Theme-Environmental Visualization (No. 013, pp. 25-31). Linköping University Electronic Press.
2. Hsu, S.C. and Wong, TT 1995 Visual Simulation of Dust Accumulation, IEEE Computer Graphics and Applications 15, 1, 18-22.
3. Fearing, P., 2000, July. Computer modelling of fallen snow. In Proceedings of the 27th annual conference on Computer graphics and interactive techniques (pp. 37-46). ACM Press/Addison-Wesley Publishing Co.



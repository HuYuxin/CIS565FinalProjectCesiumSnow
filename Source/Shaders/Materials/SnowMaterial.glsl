uniform sampler2D image;

czm_material czm_getMaterial(czm_materialInput materialInput)
{
    czm_material material = czm_getDefaultMaterial(materialInput);
	
	//Added by Yuxin For Test
	//vec3 normal = normalize(materialInput.normalEC);
	float snowSlope = materialInput.slope;
	material.alpha = snowSlope;
	material.diffuse = vec3(0.8, 0.8, 0.9);
	//material.diffuse = materialInput.normalEC;
    //material.alpha = 1.0;
	
    return material;
}
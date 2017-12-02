uniform sampler2D normalMap;
uniform sampler2D whiteNoise;

czm_material czm_getMaterial(czm_materialInput materialInput)
{
    czm_material material = czm_getDefaultMaterial(materialInput);

	//Added by Yuxin For Test
	float snowSlope = materialInput.slope;
    material.alpha = snowSlope;
	material.diffuse = vec3(0.8, 0.8, 0.9);
    vec3 normalEC = materialInput.normalEC;
    material.shininess = 200.0;

    vec3 normalMapNormal = texture2D(normalMap, materialInput.st).rgb;
    mat3 tangentToEye = materialInput.tangentToEyeMatrix;
    material.normal  = tangentToEye * normalMapNormal;
    material.specular = 0.9;
    return material;
}






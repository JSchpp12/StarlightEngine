#version 450 

layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec2 inFragTextureCoordinate; 
layout(location = 2) in vec3 inFragPositionWorld;
layout(location = 3) in vec3 inFragNormalWorld;
layout(location = 4) in vec3 inFragMatAmbient; 
layout(location = 5) in vec3 inFragMatDiffuse; 
layout(location = 6) in vec3 inFragMatSpecular; 
layout(location = 7) in float inFragMatShininess; 

layout(location = 0) out vec4 outColor;

struct RenderSettings{
	uint draw;				//standard draw
	uint drawMatAmbient;  
	uint drawMatDiffuse; 
	uint drawMatSpecular; 
};

struct Light{
	vec4 position;

	//properties
	vec4 ambient; 
	vec4 diffuse;
	vec4 specular; 
};

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	mat4 inverseView; 
	uint numLights; 
	uint renderSettings;
} globalUbo; 

 layout(binding = 1, set = 0) buffer globalLightBuffer{
	Light lights[];
 };

// layout(binding = 0, set = 2) uniform ObjectRenderingOptions{
//	bool useTexture;
//};

layout(binding = 0, set = 2) uniform sampler2D textureSampler; 

RenderSettings createSettingsStruct(){
	RenderSettings settingsChecker = {
		0, 
		1, 
		2, 
		3
	};
	return(settingsChecker); 
}
void main() {
	RenderSettings settingsChecker = createSettingsStruct(); 

	vec3 ambientLight = vec3(0.0); 
	vec3 diffuseLight = vec3(0.0);  
	vec3 specularLight = vec3(0.0);															//container for summation of light contributions to specular lighting result
	vec3 surfaceNormal = normalize(inFragNormalWorld);										//using same normal for all frags on surface -- rather than calculating for each one

	vec3 cameraPosWorld = globalUbo.inverseView[3].xyz; 
	vec3 viewDirection = normalize(cameraPosWorld - inFragPositionWorld); 

	if ((globalUbo.renderSettings & settingsChecker.drawMatAmbient) != 0){
		//apply frag ambient value 
		outColor = vec4(inFragMatAmbient, 1.0); 
	}else if ((globalUbo.renderSettings & settingsChecker.drawMatDiffuse) != 0){
		outColor = vec4(inFragMatDiffuse, 1.0); 
	}else if ((globalUbo.renderSettings & settingsChecker.drawMatSpecular) != 0){
		outColor = vec4(inFragMatSpecular, 1.0); 
	}else{
		for (int i = 0; i < globalUbo.numLights; i++){
			//distance calculations 
			vec3 directionToLight = lights[i].position.xyz - inFragPositionWorld.xyz; 
			float attenuation = 1.0 / dot(directionToLight, directionToLight);					//distance of direction vector squared

			//ambient light 
			ambientLight += (lights[i].ambient.xyz * lights[i].ambient.w) * attenuation; 

			//need to normalize this after the attenuation calculation 
			directionToLight = normalize(directionToLight); 

			float cosAngleIncidence = max(dot(surfaceNormal, directionToLight), 0);
			vec3 lightColor = lights[i].ambient.xyz * lights[i].ambient.w * attenuation;

			//diffuse light 
			diffuseLight += (lightColor * cosAngleIncidence) * attenuation; 

			//specular light
			vec3 halfAngle = normalize(directionToLight + viewDirection); 
			float blinnTerm = dot(surfaceNormal, halfAngle); 
			blinnTerm = clamp(blinnTerm, 0, 1);	
			blinnTerm = cosAngleIncidence != 0.0 ? blinnTerm : 0; 
			//apply arbitrary power "s" -- high values results in sharper highlight
			blinnTerm = pow(blinnTerm, inFragMatShininess); 

			specularLight += ((lights[i].specular.xyz * lights[i].specular.w) * blinnTerm) * attenuation; 
		}

	ambientLight *= inFragMatAmbient; 
	diffuseLight *= inFragMatDiffuse; 
	specularLight *= inFragMatSpecular; 

	vec3 totalSurfaceColor = (ambientLight + diffuseLight + specularLight) * vec3(texture(textureSampler, inFragTextureCoordinate)); 
	outColor = vec4(totalSurfaceColor, 1.0);
	}
}
#version 450 

//expecting texture sampler
//layout(binding = 1) uniform sampler2D texSampler; 

layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec3 inFragPositionWorld;
layout(location = 2) in vec3 inFragNormalWorld;

//layout(location = 1) in vec2 fragTexCoord; 

layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	mat4 inverseView; 
	vec4 ambientLightColor; 
	int numLights; 
} globalUbo; 

layout(binding = 1, set = 0) buffer uniformLightPositions{
	vec4 values[];
} lightPositions;

layout(binding = 2, set = 0) buffer uniformLightColors{
	vec4 values[];
} lightColors; 

void main() {
	vec3 diffuseLight = globalUbo.ambientLightColor.xyz * globalUbo.ambientLightColor.w; 
	vec3 surfaceNormal = normalize(inFragNormalWorld);				//using same normal for all frags on surface -- rather than calculating for each one

//	outColor = texture(texSampler, fragTexCoord);
	for (int i = 0; i < globalUbo.numLights; i++){
		//light calculations -- difuse lighting
		vec3 directionToLight = lightPositions.values[i].xyz - inFragPositionWorld.xyz; 
		float attenuation = 1.0 / dot(directionToLight, directionToLight);	//distance of direction vector squared
		float cosAngleIncidence = max(dot(surfaceNormal, normalize(directionToLight)), 0);
		vec3 intensity = lightColors.values[i].xyz * lightColors.values[i].w * attenuation;

		diffuseLight += intensity * cosAngleIncidence; 
	}
	vec3 ambientLight = globalUbo.ambientLightColor.xyz * globalUbo.ambientLightColor.w; 

	outColor = vec4((diffuseLight + ambientLight) * inFragColor, 1.0); 
}
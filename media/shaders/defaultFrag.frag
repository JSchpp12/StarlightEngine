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
	vec4 ambientLightColor; 
} globalUbo; 

layout(binding = 1, set = 0) uniform uniformLightPositions{
	vec4 value; 
} lightPositions;

layout(binding = 2, set = 0) uniform uniformLightColors{
	vec4 value; 
} lightColors; 

void main() {
//	outColor = texture(texSampler, fragTexCoord);

	//light calculations
	vec3 directionToLight = lightPositions.value.xyz - inFragPositionWorld.xyz; 
	float attenuation = 1.0 / dot(directionToLight, directionToLight);	//distance of direction vector squared

	//apply scaling to light intensities
	vec3 lightColor = lightColors.value.xyz * lightColors.value.w * attenuation; 
	vec3 ambientLight = globalUbo.ambientLightColor.xyz * globalUbo.ambientLightColor.w; 

	//this calculation only works if both vectors are normalized
	//linear interpolation of two normal vectors, is not always normal
	vec3 diffuseLight = lightColor * max(dot(normalize(inFragNormalWorld), normalize(directionToLight)), 0); 

	outColor = vec4((diffuseLight + ambientLight) * inFragColor, 1.0); 
}
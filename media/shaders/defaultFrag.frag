#version 450 

layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec2 inFragTextureCoordinate; 
layout(location = 2) in vec3 inFragPositionWorld;
layout(location = 3) in vec3 inFragNormalWorld;

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

layout(binding = 0, set = 2) buffer  bufferObjectMaterial{
	vec4 surfaceColor; 
	vec4 highlightColor; 
	int shinyCoefficient; 
} objectMaterial; 

layout(binding = 1, set = 2) uniform sampler2D textureSampler; 

void main() {
//	vec3 diffuseLight = globalUbo.ambientLightColor.xyz * globalUbo.ambientLightColor.w; 
//	vec3 specularLight = vec3(0.0);															//container for summation of light contributions to specular lighting result
//	vec3 surfaceNormal = normalize(inFragNormalWorld);										//using same normal for all frags on surface -- rather than calculating for each one
//
//
//	vec3 cameraPosWorld = globalUbo.inverseView[3].xyz; 
//	vec3 viewDirection = normalize(cameraPosWorld - inFragPositionWorld); 
//
////	outColor = texture(texSampler, fragTexCoord);
//	for (int i = 0; i < globalUbo.numLights; i++){
//		//diffuse lighting calculation
//		vec3 directionToLight = lightPositions.values[i].xyz - inFragPositionWorld.xyz; 
//		float attenuation = 1.0 / dot(directionToLight, directionToLight);					//distance of direction vector squared
//
//		//need to normalize this after the attenuation calculation 
//		directionToLight = normalize(directionToLight); 
//
//		float cosAngleIncidence = max(dot(surfaceNormal, directionToLight), 0);
//		vec3 intensity = lightColors.values[i].xyz * lightColors.values[i].w * attenuation;
//
//		diffuseLight += intensity * cosAngleIncidence; 
//
//		//specular lighting calculation 
//		vec3 halfAngle = normalize(directionToLight + viewDirection); 
//		float blinnTerm = dot(surfaceNormal, halfAngle); 
//		blinnTerm = clamp(blinnTerm, 0, 1);	
//		//apply arbitrary power "s" -- high values results in sharper highlight
//		blinnTerm = pow(blinnTerm, objectMaterial.shinyCoefficient); 
//
//		specularLight += intensity * blinnTerm; 
//
//	}
//	vec3 ambientLight = globalUbo.ambientLightColor.xyz * globalUbo.ambientLightColor.w; 
//	vec3 surfaceColor = objectMaterial.surfaceColor.xyz * objectMaterial.surfaceColor.w; 
//	vec3 highlightColor = objectMaterial.highlightColor.xyz * objectMaterial.highlightColor.w; 
//
//	//second multiplication of frag color is a placeholder for control term of highlight and specular color 
//	outColor = vec4(surfaceColor * diffuseLight + specularLight * highlightColor, 1.0); 

	outColor = texture(textureSampler, inFragTextureCoordinate);
}
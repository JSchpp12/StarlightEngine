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
	int numLights; 
} globalUbo; 

 layout(binding = 1, set = 0) buffer globalLightBuffer{
	Light lights[];
 };

layout(binding = 1, set = 2) uniform sampler2D textureSampler; 

void main() {
	vec3 ambientLight = vec3(0.0); 
	vec3 diffuseLight = vec3(0.0);  
	vec3 specularLight = vec3(0.0);															//container for summation of light contributions to specular lighting result
	vec3 surfaceNormal = normalize(inFragNormalWorld);										//using same normal for all frags on surface -- rather than calculating for each one

	vec3 cameraPosWorld = globalUbo.inverseView[3].xyz; 
	vec3 viewDirection = normalize(cameraPosWorld - inFragPositionWorld); 

	for (int i = 0; i < globalUbo.numLights; i++){
		//ambient light 
		ambientLight += (lights[i].ambient.xyz * lights[i].ambient.w); 

		//diffuse lighting calculation
		vec3 directionToLight = lights[i].position.xyz - inFragPositionWorld.xyz; 
		float attenuation = 1.0 / dot(directionToLight, directionToLight);					//distance of direction vector squared

		//need to normalize this after the attenuation calculation 
		directionToLight = normalize(directionToLight); 

		float cosAngleIncidence = max(dot(surfaceNormal, directionToLight), 0);
		vec3 lightColor = lights[i].ambient.xyz * lights[i].ambient.w * attenuation;

		diffuseLight += lightColor * cosAngleIncidence; 

		//specular lighting calculation 
		vec3 halfAngle = normalize(directionToLight + viewDirection); 
		float blinnTerm = dot(surfaceNormal, halfAngle); 
		blinnTerm = clamp(blinnTerm, 0, 1);	
		blinnTerm = cosAngleIncidence != 0.0 ? blinnTerm : 0; 
		//apply arbitrary power "s" -- high values results in sharper highlight
		blinnTerm = pow(blinnTerm, inFragMatShininess); 

		specularLight += (lights[i].specular.xyz * lights[i].specular.w) * blinnTerm; 

	}
	ambientLight *= inFragMatAmbient; 
	diffuseLight *= inFragMatDiffuse; 
	specularLight *= inFragMatSpecular; 


	vec3 totalSurfaceColor = (ambientLight + diffuseLight + specularLight) * vec3(texture(textureSampler, inFragTextureCoordinate)); 
	outColor = vec4(totalSurfaceColor, 1.0); 
//	outColor = vec4(inFragMatAmbient, 1.0); 
}
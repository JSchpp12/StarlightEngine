#version 450 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal; 
layout(location = 2) in vec3 inColor;		//vertex color
layout(location = 3) in vec2 inTexCoord;	//texture coordinate for vertex 

layout(location = 0) out vec3 fragColor; 
layout(location = 1) out vec2 fragTexCoord; 

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	vec4 ambientLightColor; 
	vec3 lightPosition; 
	vec4 lightColor;			//w is intensity
} globalUbo; 

layout(binding = 0, set = 1) uniform UniformBufferObject{
	mat4 modelMatrix; 
	mat4 normalModelMatrix; 
} objectUbo;

void main() {
	vec4 positionWorldSpace = objectUbo.modelMatrix * vec4(inPosition, 1.0); 

	//calculate direction to the light source from the vertex
	vec3 directionToLight = globalUbo.lightPosition - positionWorldSpace.xyz; 

	//have to convert light location from model space to world space --TMP--
	vec3 normalWorldSpace = normalize(mat3(objectUbo.normalModelMatrix)  * inNormal); 

	//apply scaling to light intensities
	vec3 lightColor = globalUbo.lightColor.xyz * globalUbo.lightColor.w; 
	vec3 ambientLight = globalUbo.ambientLightColor.xyz * globalUbo.ambientLightColor.w; 
	vec3 diffuseLight = lightColor * max(dot(normalWorldSpace, normalize(directionToLight)), 0); 

	gl_Position = globalUbo.proj * globalUbo.view * positionWorldSpace;
	fragColor = (diffuseLight + ambientLight) * inColor; 

	fragTexCoord = inTexCoord; 
}
#version 450 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal; 
layout(location = 2) in vec3 inColor;		//vertex color
layout(location = 3) in vec2 inTexCoord;	//texture coordinate for vertex 

layout(location = 0) out vec3 fragColor; 
layout(location = 1) out vec3 fragPositionWorld;	//fragment's position in world space
layout(location = 2) out vec3 fragNormalWorld;		//fragment's normal in world space 

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	vec4 ambientLightColor; 
	int numLights; 
} globalUbo; 

layout(binding = 1, set = 0) uniform uniformLightPositions{
	vec3 value; 
} lightPositions;

layout(binding = 2, set = 0) uniform uniformLightColors{
	vec4 value; 
} lightColors; 

//can varry material by vertex 

layout(binding = 0, set = 1) uniform UniformBufferObject{
	mat4 modelMatrix; 
	mat4 normalModelMatrix; 
} objectUbo;

void main() {
	vec4 positionWorld = objectUbo.modelMatrix * vec4(inPosition, 1.0); 
	gl_Position = globalUbo.proj * globalUbo.view * positionWorld;
	fragNormalWorld = normalize(mat3(objectUbo.normalModelMatrix) * inNormal); 
	fragPositionWorld = positionWorld.xyz; 
	fragColor = inColor; 
//	fragTexCoord = inTexCoord; 
}
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
	mat4 inverseView; 
	vec4 ambientLightColor; 
	int numLights; 
} globalUbo; 

layout(binding = 1, set = 0) buffer uniformLightPositions{
	vec3 values[]; 
} lightInfo;

layout(binding = 2, set = 0) buffer uniformLightColors{
	vec4 values[]; 
} lightColors; 

//can varry material by vertex 
layout(binding = 0, set = 1) uniform uniformBufferObject{
	mat4 modelMatrix; 
	mat4 normalModelMatrix; 
} objectUbo;

layout(binding = 0, set = 2) buffer  bufferObjectMaterial{
	vec4 surfaceColor; 
	vec4 highlightColor; 
	int shinyCoefficient; 
} objectMaterial; 

void main() {
	vec4 positionWorld = objectUbo.modelMatrix * vec4(inPosition, 1.0); 
	gl_Position = globalUbo.proj * globalUbo.view * positionWorld;
	fragNormalWorld = normalize(mat3(objectUbo.normalModelMatrix) * inNormal);
	fragPositionWorld = positionWorld.xyz; 
	fragColor = inColor; 
//	fragTexCoord = inTexCoord; 
}
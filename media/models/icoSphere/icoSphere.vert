#version 450 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal; 
layout(location = 2) in vec3 inColor;		//vertex color
layout(location = 3) in vec2 inTexCoord;	//texture coordinate for vertex 

layout(location = 0) out vec3 fragColor; 

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	vec4 ambientLightColor; 
	int numLights; 
} globalUbo; 

layout(binding = 0, set = 1) uniform UniformBufferObject{
	mat4 modelMatrix; 
	mat4 normalModelMatrix; 
	vec4 color; 
} objectUbo;

void main() {
	vec4 positionWorld = objectUbo.modelMatrix * vec4(inPosition, 1.0); 
	gl_Position = globalUbo.proj * globalUbo.view * positionWorld;
	fragColor = objectUbo.color.xyz; 
}
#version 450 

layout(location = 0) in vec3 inPosition;

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	mat4 inverseView; 
	int numLights;
	int renderSettings; 
} globalUbo; 

//TODO: there needs to be one of these for each swap chain image
layout(binding = 0, set = 1) uniform uniformBufferObject{
	mat4 modelMatrix; 
} objectUbo;

layout(binding = 1, set = 1) uniform uniformLightObject{
	mat4 view; 
} lightUbo; 

void main() {
	vec4 positionWorld = objectUbo.modelMatrix * vec4(inPosition, 1.0); 
	gl_Position = globalUbo.proj * lightUbo.view * positionWorld; 
}
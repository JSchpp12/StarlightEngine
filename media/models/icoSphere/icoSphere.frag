#version 450 

layout(location = 0) in vec3 inFragColor;

layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	mat4 inverseView; 
	int numLights; 
} globalUbo; 

void main() {
	outColor = vec4(inFragColor, 1.0); 
}
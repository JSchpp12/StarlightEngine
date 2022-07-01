#version 450 

//expecting texture sampler
//layout(binding = 1) uniform sampler2D texSampler; 

layout(location = 0) in vec3 inFragColor;

//layout(location = 1) in vec2 fragTexCoord; 

layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	mat4 inverseView; 
	vec4 ambientLightColor; 
	int numLights; 
} globalUbo; 

void main() {
	outColor = vec4(inFragColor, 1.0); 
}
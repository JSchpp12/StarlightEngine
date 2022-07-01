#version 450 

layout (location = 0) in vec2 fragOffset; 
layout (location = 0) out vec4 outColor; 

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	vec4 ambientLightColor; 
	vec3 lightPosition; 
	vec4 lightColor;			//w is intensity
} globalUbo; 

void main(){
	outColor = vec4(globalUbo.lightColor.xyz, 1.0); 
//	outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
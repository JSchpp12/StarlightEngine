#version 450 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;		//vertex color
layout(location = 2) in vec2 inTexCoord;	//texture coordinate for vertex 

layout(location = 0) out vec3 fragColor; 
layout(location = 1) out vec2 fragTexCoord; 

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
} globalUbo; 

layout(binding = 0, set = 1) uniform UniformBufferObject{
	mat4 model; 
} objectUbo;
 

void main() {
	gl_Position = globalUbo.proj * globalUbo.view * objectUbo.model * vec4(inPosition, 1.0);

	fragTexCoord = inTexCoord; 
}
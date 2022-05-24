//vert shader that takes color from the ubo and passes it forward
#version 450 

layout(binding = 0) uniform UniformBufferObject {
	mat4 model; 
	mat4 view; 
	mat4 proj; 
} ubo; 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;		//vertex color
layout(location = 2) in vec2 inTexCoord;	//texture coordinate for vertex 

layout(location = 0) out vec3 fragColor; 
layout(location = 1) out vec2 fragTexCoord; 
 

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

	fragTexCoord = inTexCoord; 
}
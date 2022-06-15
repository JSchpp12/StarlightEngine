#version 450 

layout(binding = 0) uniform UniformBufferObject {
	mat4 model; 
	mat4 view; 
	mat4 proj; 
} ubo; 

//expecting texture sampler
//layout(binding = 1) uniform sampler2D texSampler; 

//output variables have to be specifically defined 
//the location value specifies the index of the framebuffer that the variable is for
layout(location = 0) in vec3 fragColor;
//layout(location = 1) in vec2 fragTexCoord; 

layout(location = 0) out vec4 outColor;

void main() {
//	outColor = texture(texSampler, fragTexCoord);
	outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
#version 450

layout(binding = 0) uniform UniformBufferObject {
	mat4 model; 
	mat4 view; 
	mat4 proj; 
	//TODO: Move this to a seperate block
	bool useGeneratedColor;		//should the vertex color be set from the colors passed through the UBO
	bool useVertexColors;
	bool useTextures; 
	vec3 color1; 
	vec3 color2; 
	vec3 color3; 
	vec3 color4;
	vec3 color5;
	vec3 color6; 
	vec3 color7; 
	vec3 color8; 
} ubo; 

layout(binding = 1) uniform sampler2D texSampler; 

//output variables have to be specifically defined 
//the location value specifies the index of the framebuffer that the variable is for
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord; 

layout(location = 0) out vec4 outColor;

void main() {
	if (ubo.useGeneratedColor || ubo.useVertexColors){
		//use color passed from vertex shader
		outColor = vec4(fragColor, 1.0f); 
	}else{
		//use texture 
		outColor = texture(texSampler, fragTexCoord); 
	}
}
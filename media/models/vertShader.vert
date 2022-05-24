//vert shader that takes color from the ubo and passes it forward
#version 450 

layout(binding = 0) uniform UniformBufferObject {
	mat4 model; 
	mat4 view; 
	mat4 proj; 
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

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;		//vertex color
layout(location = 2) in vec2 inTexCoord;	//texture coordinate for vertex 

layout(location = 0) out vec3 fragColor; 
layout(location = 1) out vec2 fragTexCoord; 
 

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	if (ubo.useGeneratedColor == true){
		if (gl_VertexIndex == 0) {
			fragColor = ubo.color1; 
		}else if (gl_VertexIndex == 1){
			fragColor = ubo.color2;
		}else if (gl_VertexIndex == 2){
			fragColor = ubo.color3; 
		}else if (gl_VertexIndex == 3){
			fragColor = ubo.color4; 
		}else if (gl_VertexIndex == 4){
			fragColor = ubo.color5; 
		}else if (gl_VertexIndex == 5){
			fragColor = ubo.color6; 
		}else if (gl_VertexIndex == 6){
			fragColor = ubo.color7; 
		}else if (gl_VertexIndex == 7){
			fragColor = ubo.color8;
		}else{
			//default to vbo color
			fragColor = inColor; 
		}
	}else{
		fragColor = inColor; 
	}

	fragTexCoord = inTexCoord; 
}
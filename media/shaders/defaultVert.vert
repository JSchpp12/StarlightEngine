#version 450 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal; 
layout(location = 2) in vec3 inColor;		//vertex color
layout(location = 3) in vec2 inTexCoord;	//texture coordinate for vertex 
layout(location = 4) in vec3 inMatAmbient; 
layout(location = 5) in vec3 inMatDiffuse; 
layout(location = 6) in vec3 inMatSpecular; 
layout(location = 7) in float inMatShininess; 

struct light{
	vec4 position;

	//properties
	vec4 ambient; 
	vec4 diffuse;
	vec4 specular; 
};

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	mat4 inverseView; 
	int numLights; 
} globalUbo; 

//can varry material by vertex 
layout(binding = 0, set = 1) uniform uniformBufferObject{
	mat4 modelMatrix; 
	mat4 normalModelMatrix; 
} objectUbo;

layout(location = 0) out vec3 fragColor; 
layout(location = 1) out vec2 fragTextureCoordinate; 
layout(location = 2) out vec3 fragPositionWorld;	//fragment's position in world space
layout(location = 3) out vec3 fragNormalWorld;		//fragment's normal in world space 
layout(location = 4) out vec3 outFragMatAmbient; 
layout(location = 5) out vec3 outFragMatDiffuse; 
layout(location = 6) out vec3 outFragMatSpecular; 
layout(location = 7) out float outFragMatShininess; 

void main() {
	vec4 positionWorld = objectUbo.modelMatrix * vec4(inPosition, 1.0); 
	gl_Position = globalUbo.proj * globalUbo.view * positionWorld;
	fragNormalWorld = normalize(mat3(objectUbo.normalModelMatrix) * inNormal);
	fragPositionWorld = positionWorld.xyz; 
	fragColor = inColor; 
	fragTextureCoordinate = inTexCoord; 
	outFragMatAmbient = inMatAmbient; 
	outFragMatDiffuse = inMatDiffuse; 
	outFragMatSpecular = inMatSpecular; 
	outFragMatShininess = inMatShininess; 
}
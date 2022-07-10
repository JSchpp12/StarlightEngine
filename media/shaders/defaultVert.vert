#version 450 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal; 
layout(location = 2) in vec3 inColor;		//vertex color
layout(location = 3) in vec2 inTexCoord;	//texture coordinate for vertex 
layout(location = 4) in vec3 inMaterialAmbient; 
layout(location = 5) in vec3 inMatDiffuse; 
layout(location = 6) in vec3 inMaterialSpecular; 
layout(location = 7) in float shininess;

struct light{
	vec4 position;
	vec4 diffuseColor;
	vec4 ambientColor; 
};

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	mat4 inverseView; 
	vec4 ambientLightColor; 
	int numLights; 
} globalUbo; 

//can varry material by vertex 
layout(binding = 0, set = 1) uniform uniformBufferObject{
	mat4 modelMatrix; 
	mat4 normalModelMatrix; 
} objectUbo;

layout(binding = 0, set = 2) buffer bufferObjectMaterial{
	vec4 surfaceColor; 
	vec4 highlightColor; 
	int shinyCoefficient; 
} objectMaterial; 

layout(location = 0) out vec3 fragColor; 
layout(location = 1) out vec2 fragTextureCoordinate; 
layout(location = 2) out vec3 fragPositionWorld;	//fragment's position in world space
layout(location = 3) out vec3 fragNormalWorld;		//fragment's normal in world space 
layout(location = 4) out vec3 outMatAmbient; 
layout(location = 5) out vec3 outMatDiffuse; 
layout(location = 6) out float outMatShininess; 

void main() {
	vec4 positionWorld = objectUbo.modelMatrix * vec4(inPosition, 1.0); 
	gl_Position = globalUbo.proj * globalUbo.view * positionWorld;
	fragNormalWorld = normalize(mat3(objectUbo.normalModelMatrix) * inNormal);
	fragPositionWorld = positionWorld.xyz; 
	fragColor = inColor; 
	fragTextureCoordinate = inTexCoord; 
}
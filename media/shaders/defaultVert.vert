#version 450 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal; 
layout(location = 2) in vec3 inColor;		//vertex color
layout(location = 3) in vec2 inTexCoord;	//texture coordinate for vertex 
layout(location = 4) in vec3 inTangent; 
layout(location = 5) in vec3 inBiTangent; 
//per veterx materials
layout(location = 6) in vec3 inMatAmbient; 
layout(location = 7) in vec3 inMatDiffuse; 
layout(location = 8) in vec3 inMatSpecular; 
layout(location = 9) in float inMatShininess; 

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	mat4 inverseView; 
	int numLights;
	int renderSettings; 
} globalUbo; 

//TODO: there needs to be one of these for each swap chain image
layout(binding = 0, set = 1) uniform uniformBufferObject{
	mat4 modelMatrix; 
	mat4 normalModelMatrix; 
} objectUbo;

layout(location = 0) out vec3 outFragColor; 
layout(location = 1) out vec2 outFragTextureCoordinate; 
layout(location = 2) out vec3 outFragPositionWorld;	//fragment's position in world space
layout(location = 3) out vec3 outFragNormalWorld;		//fragment's normal in world space 
layout(location = 4) out vec3 outFragMatAmbient; 
layout(location = 5) out vec3 outFragMatDiffuse; 
layout(location = 6) out vec3 outFragMatSpecular; 
layout(location = 7) out float outFragMatShininess; 
layout(location = 8) out mat3 outTBNMat; 


void main() {
	vec4 positionWorld = objectUbo.modelMatrix * vec4(inPosition, 1.0); 

	//calculate TBN mat for use from translating sampled normal matrix from Tangent space to model space
	vec3 T = normalize(vec3(objectUbo.modelMatrix * vec4(inTangent, 0.0)));
	vec3 B = normalize(vec3(objectUbo.modelMatrix * vec4(inBiTangent, 0.0)));
	vec3 N = normalize(vec3(objectUbo.modelMatrix * vec4(inNormal, 0.0))); 
	//Gram-Schmidt process to re-orthogonalize TBN vectors
//	T = normalize(T - dot(T, N) * N
	outTBNMat = mat3(T, B, N); 

	gl_Position = globalUbo.proj * globalUbo.view * positionWorld;
	outFragNormalWorld = normalize(mat3(objectUbo.normalModelMatrix) * inNormal);

	//pass through needed properties to fragments
	outFragPositionWorld = positionWorld.xyz; 
	outFragColor = inColor; 
	outFragTextureCoordinate = inTexCoord; 
	outFragMatAmbient = inMatAmbient; 
	outFragMatDiffuse = inMatDiffuse; 
	outFragMatSpecular = inMatSpecular; 
	outFragMatShininess = inMatShininess; 
}
#version 450 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal; 
layout(location = 2) in vec3 inColor;		//vertex color
layout(location = 3) in vec2 inTexCoord;	//texture coordinate for vertex 

layout(location = 0) out vec3 fragColor; 
layout(location = 1) out vec2 fragTexCoord; 

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
} globalUbo; 

layout(binding = 0, set = 1) uniform UniformBufferObject{
	mat4 modelMatrix; 
	mat4 normalModelMatrix; 
} objectUbo;
 
const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT = 0.02f;

void main() {
	vec4 positionWorldSpace = objectUbo.modelMatrix * vec4(inPosition, 1.0); 

	//have to convert light location from model space to world space --TMP--
	vec3 normalWorldSpace = normalize(mat3(objectUbo.normalModelMatrix)  * inNormal); 

	gl_Position = globalUbo.proj * globalUbo.view * positionWorldSpace;

	float lightIntensity = AMBIENT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

	fragColor = lightIntensity * inColor; 
	fragTexCoord = inTexCoord; 
}
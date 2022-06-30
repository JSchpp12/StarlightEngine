#version 450 

layout(location = 0) out vec2 fragOffset;

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	vec4 ambientLightColor; 
	vec3 lightPosition; 
	vec4 lightColor;			//w is intensity
} globalUbo; 

const vec2 OFFSETS[6] = vec2[](
  vec2(-5.0, -5.0),
  vec2(-5.0, 5.0),
  vec2(5.0, -5.0),
  vec2(5.0, -5.0),
  vec2(-5.0, 5.0),
  vec2(5.0, 5.0)
);

const float LIGHT_RADIUS = 500.0; 

void main() {
	fragOffset = OFFSETS[gl_VertexIndex]; 

	vec3 cameraRightWorld = {globalUbo.view[0][0], globalUbo.view[1][0], globalUbo.view[2][0]}; 
	vec3 cameraUpWorld = {globalUbo.view[0][1], globalUbo.view[1][1], globalUbo.view[2][1]}; 
	
	vec3 positionWorld = globalUbo.lightPosition.xyz 
		+ LIGHT_RADIUS * fragOffset.x
		+ LIGHT_RADIUS * fragOffset.y; 

//	vec3 positionWorld = vec3(3.0, 3.0, 3.0);

    gl_Position = globalUbo.proj * globalUbo.view * vec4(positionWorld, 1.0); 
}
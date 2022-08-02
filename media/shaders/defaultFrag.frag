#version 450 

layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec2 inFragTextureCoordinate; 
layout(location = 2) in vec3 inFragPositionWorld;
layout(location = 3) in vec3 inFragNormalWorld;
layout(location = 4) in vec3 inFragMatAmbient; 
layout(location = 5) in vec3 inFragMatDiffuse; 
layout(location = 6) in vec3 inFragMatSpecular; 
layout(location = 7) in float inFragMatShininess;
layout(location = 8) in mat3 inTBNMat;

layout(location = 0) out vec4 outColor;

struct RenderSettings{
	uint draw;				//standard draw
	uint drawMatAmbient;  
	uint drawMatDiffuse; 
	uint drawMatSpecular; 
	uint drawMapBump; 

	//rendering features
	uint bumpMapping; 
};

struct Light_Type{
	uint point; 
	uint directional; 
	uint spot;
};

struct Light{
	vec4 position;
	vec4 direction;

	//properties
	vec4 ambient; 
	vec4 diffuse;
	vec4 specular; 
	//controls.x = inner cutoff angle 
	//controls.y = outer cutoff angle
	vec4 controls; 
	//settings.x = enabled
	//settings.y = type
	uvec4 settings; 
};

layout(binding = 0, set = 0) uniform GlobalUniformBufferObject {
	mat4 proj;
	mat4 view;  
	mat4 inverseView; 
	uint numLights; 
	uint renderSettings;
} globalUbo; 

 layout(binding = 1, set = 0) buffer globalLightBuffer{
	Light lights[];
 };

layout(binding = 0, set = 2) uniform sampler2D textureSampler; 
layout(binding = 1, set = 2) uniform sampler2D normalMapSampler; 

//TODO: move struct init to a different place - not good idea to do this for every fragment
RenderSettings createSettingsStruct(){
	RenderSettings settingsChecker = {
		0x0, 
		0x1,
		0x2,
		0x3,
		0x4, 
		//render features
		0x10000
	};
	return(settingsChecker); 
}
//TODO: same move here 
Light_Type createLightTypeStruct(){
	Light_Type lightChecker = {
		0x0, 
		0x1, 
		0x2
	};
	return(lightChecker);
}

void lightCalculations(Light light, inout vec3 fragColor){

}

void main() {
	bool isSpot = false;
	bool isDirectional = false; 
	vec3 rawDiffuse = vec3(0.0);		//tmp storage for diffuse lighting calculation result before applying modifiers
	vec3 rawSpecular = vec3(0.0);		//tmp storage for specular lighting calculation result before applying modifiers
	RenderSettings settingsChecker = createSettingsStruct(); 
	Light_Type lightChecker = createLightTypeStruct(); 

	vec3 ambientLight = vec3(0.0); 
	vec3 diffuseLight = vec3(0.0);  
	vec3 specularLight = vec3(0.0);															//container for summation of light contributions to specular lighting result
	vec3 surfaceNormal = normalize(inFragNormalWorld);										//using same normal for all frags on surface -- rather than calculating for each one

	//Bump Mapping Calculations
	//check if the sampled normal is 0, as this means that no bump map was supplied...skip calculations if so
	vec3 sampledNormal = texture(normalMapSampler, inFragTextureCoordinate).rgb; 
	if (((globalUbo.renderSettings & settingsChecker.bumpMapping) != 0) && sampledNormal.r != 0 && sampledNormal.g != 0 && sampledNormal.b != 0){
		sampledNormal = sampledNormal * 2.0 - 1.0; 
		surfaceNormal = normalize(inTBNMat * sampledNormal); 
	}

	vec3 cameraPosWorld = globalUbo.inverseView[3].xyz; 
	vec3 viewDirection = normalize(cameraPosWorld - inFragPositionWorld); 

	if ((globalUbo.renderSettings & settingsChecker.drawMatAmbient) != 0){
		//apply frag ambient value 
		outColor = vec4(inFragMatAmbient, 1.0); 
	}else if ((globalUbo.renderSettings & settingsChecker.drawMatDiffuse) != 0){
		outColor = vec4(inFragMatDiffuse, 1.0); 
	}else if ((globalUbo.renderSettings & settingsChecker.drawMatSpecular) != 0){
		outColor = vec4(inFragMatSpecular, 1.0); 
	}else if ((globalUbo.renderSettings & settingsChecker.drawMapBump) != 0){
		outColor = vec4(vec3(texture(normalMapSampler, inFragTextureCoordinate)), 1.0);
	}else{
		for (int i = 0; i < globalUbo.numLights; i++){
			//check if the current light object is a spotlight
			isSpot = ((lights[i].settings.y & lightChecker.spot) != 0);
			isDirectional = ((lights[i].settings.y & lightChecker.directional) != 0);

			//distance calculations 
			if (lights[i].settings.x == 1){
				//light is enabled
				vec3 directionToLight = lights[i].position.xyz - inFragPositionWorld.xyz; 
				if ((lights[i].settings.y & lightChecker.directional) != 0){
					//Directional light 
					directionToLight = normalize(-lights[i].direction.xyz); 
				}else {
					directionToLight = lights[i].position.xyz - inFragPositionWorld.xyz; 
				}
				
				//distance of direction vector squared
				float attenuation = 1.0 / dot(directionToLight, directionToLight);	

				//apply ambient light (no attenuation for ambient sources)
				ambientLight += !isDirectional ? (lights[i].ambient.xyz * lights[i].ambient.w) * attenuation : lights[i].ambient.xyz * lights[i].ambient.w; 

				//need to normalize this after the attenuation calculation 
				directionToLight = normalize(directionToLight); 

				//calculate cosine value of difference between fragment vec to light and light direction
				float theta = dot(directionToLight, normalize(-lights[i].direction.xyz)); 
				float epsilon = lights[i].controls.x - lights[i].controls.y;						//inner cutoff - outer cutoff
				float spotIntensity = clamp((theta - lights[i].controls.y) / epsilon, 0.0, 1.0);	//want to keep intensity between 0 and 1 

				//apply lighting calculations 
				float cosAngleIncidence = max(dot(surfaceNormal, directionToLight), 0);
				vec3 lightColor = (lights[i].diffuse.xyz * lights[i].diffuse.w) * attenuation;

				rawDiffuse = lightColor * cosAngleIncidence; 
				//apply attenuation to light sources that are not directional
				if (!isDirectional)
					rawDiffuse *= attenuation; 

				//specular light
				vec3 halfAngle = normalize(directionToLight + viewDirection); 
				float blinnTerm = dot(surfaceNormal, halfAngle); 
				blinnTerm = clamp(blinnTerm, 0, 1);	
				blinnTerm = cosAngleIncidence != 0.0 ? blinnTerm : 0; 
				//apply arbitrary power "s" -- high values results in sharper highlight
				blinnTerm = pow(blinnTerm, inFragMatShininess); 

				rawSpecular = ((lights[i].specular.xyz * lights[i].specular.w) * blinnTerm); 
				//apply attenuation to light sources that are not directional
				if (!isDirectional)
					rawSpecular *= attenuation; 

				//apply any futher modifiers if needed (i.e. spot light outer ring)
				if (isSpot && (theta < lights[i].controls.x)){
					//apply outer ring intensities to fragment light effects
					diffuseLight += rawDiffuse * spotIntensity; 
					specularLight =+ rawSpecular * spotIntensity; 
				}else{
					diffuseLight += rawDiffuse; 
					specularLight += rawSpecular;
				}
			}

			ambientLight *= inFragMatAmbient; 
			diffuseLight *= inFragMatDiffuse; 
			specularLight *= inFragMatSpecular; 

			vec3 totalSurfaceColor = (ambientLight + diffuseLight + specularLight) * vec3(texture(textureSampler, inFragTextureCoordinate)); 

			outColor = vec4(totalSurfaceColor, 1.0);
		}
	}
}

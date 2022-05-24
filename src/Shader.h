#pragma once

#include "Enums.h"

#include "vulkan/vulkan.hpp"
#include <vector>

//fundamental shader object which will contain pointer to compiled shader for use in vulkan pipeline
class Shader {
public:
	Shader(); 

	std::vector<uint32_t> GetSpirV();

	vk::ShaderStageFlagBits GetStage(); 
	//Shader(const Shader&) = delete; 
	//Shader& operator = (const Shader&) = delete; 
	//~Shader() = default; 
protected: 
	std::vector<uint32_t> spirv;
	vk::ShaderStageFlagBits stages; 

private: 

};
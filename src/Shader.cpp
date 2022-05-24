#include "Shader.h"

Shader::Shader()
{

}

std::vector<uint32_t> Shader::GetSpirV()
{
	return this->spirv; 
}

vk::ShaderStageFlagBits Shader::GetStage()
{
	return this->stages; 
}

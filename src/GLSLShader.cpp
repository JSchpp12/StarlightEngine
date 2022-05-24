#include "GLSLShader.h"

GLSLShader::GLSLShader()
{
}

GLSLShader::GLSLShader(vk::ShaderStageFlagBits stage, std::string pathToFile)
{
	std::string shaderCode = this->ReadShaderFile(pathToFile);
	const char* rawShaderCode = shaderCode.c_str();
	this->Compile(pathToFile, ShadercHelper::GetShaderCStageFlag(stage), rawShaderCode);
}

void GLSLShader::Compile(std::string shaderName, shaderc_shader_kind stage, const char* shaderCode)
{
	auto preprocessed = ShadercHelper::preprocess_shader(shaderName, stage, shaderCode); 

	//compile shader
	//auto assembly = ShadercHelper::compile_file_to_assembly(shaderName, shaderc_glsl_vertex_shader, shaderCode, false);
	//std::cout << "Assembled to SPIRV" << std::endl; 

	this->spirv = ShadercHelper::compile_file(shaderName, stage, preprocessed);

	if (spirv.size() == 0) {
		//failed to compile shader 
		throw std::runtime_error("Failed to compile shader"); 
	}
	std::cout << "Compiled shader:" << shaderName << std::endl;

	//{  // Error case
	//	const char kBadShaderSource[] =
	//		"#version 310 es\nint main() { int main_should_be_void; }\n";

	//	std::cout << std::endl << "Compiling a bad shader:" << std::endl;
	//	auto result = ShadercHelper::compile_file("bad_src", shaderc_glsl_vertex_shader, kBadShaderSource);
	//}

}

EShLanguage GLSLShader::GetLanguage(STAR_SHADER_STAGE stage)
{
	//for now, using the provided argument to generate proper stage 
	//TODO: fix this
	switch (stage) {
	case(STAR_SHADER_STAGE::VERTEX):
		return EShLanguage::EShLangVertex; 
		break;
	case(STAR_SHADER_STAGE::FRAGMENT):
		return EShLanguage::EShLangFragment; 
		break;
	default:
		throw std::invalid_argument("The provided stage is not supported"); 
	}
	return EShLanguage();
}

std::string GLSLShader::ReadShaderFile(std::string pathToFile)
{
	//read in file 
	std::string line, text;
	std::ifstream fileReader(pathToFile);

	while (std::getline(fileReader, line)) {
		text += line + "\n";
	}

	return(text); 
	//shaderFile = std::unique_ptr<const char>(text.c_str());
}

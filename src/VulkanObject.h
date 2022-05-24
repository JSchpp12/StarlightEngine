#pragma once 
#include "Vertex.h"
#include "Shader.h"
#include "Enums.h"
#include "GLSlangHelper.h"
#include "GLSLShader.h"

#include <vulkan/vulkan.hpp>
#include <tiny_obj_loader.h>

#include <vector>
#include <unordered_map>

class VulkanObject {
public:
	~VulkanObject();

	VulkanObject(std::string pathToObjFile); 

	/// <summary>
	/// Ordered drawing of verticies
	/// </summary>
	/// <param name="verticies">List of verticies</param>
	/// <param name="indicies">Order of indicies that will be used during drawing</param>
	VulkanObject(std::vector<Vertex>* verticies, std::vector<uint32_t>* indicies); 

	//copy assignment 
	//VulkanObject& operator = (VulkanObject&&) = default; 

	void AddShader(vk::ShaderStageFlagBits stage, std::string pathToFile); 

	/// <summary>
	/// Add object file. Must be a .obj file
	/// </summary>
	/// <param name="pathToFile"></param>
	void AddObject(std::string pathToFile); 

	std::vector<Vertex> GetVerticies(); 

	std::vector<uint32_t> GetIndicies();

	std::vector<Shader> GetShaders(); 

	Shader GetVertShader(); 

	Shader GetFragShader(); 

	void AddTexture(std::string pathToTexture); 
protected:

	void LoadObjFile(std::string pathToFile);
private:
	std::string pathToTexture; 
	std::vector<Vertex> vertexList; 
	std::vector<uint32_t> indicies;
	GLSLShader vertexShader; 
	GLSLShader fragShader;
	//TODO: implement positional elements
#pragma region todo
	////current position of the object
	//glm::vec3 position;
	////relative rotation of the object
	//glm::vec3 relativeRotation;

	//glm::vec4 displayMatrix;
#pragma endregion
 

};
#include "VulkanObject.h"


VulkanObject::~VulkanObject()
{

}

VulkanObject::VulkanObject(std::string pathToObjFile)
{
	//TODO: ensure file is correct type 
	this->LoadObjFile(pathToObjFile);
	std::cout << "Loading complete: " << pathToObjFile << std::endl; 
}

VulkanObject::VulkanObject(std::vector<Vertex>* verticies, std::vector<uint32_t>* indicies)
{
	this->vertexList = *verticies; 
	this->indicies = *indicies; 
}

void VulkanObject::AddShader(vk::ShaderStageFlagBits stage, std::string pathToFile)
{
	if (stage == vk::ShaderStageFlagBits::eVertex) {
		this->vertexShader = GLSLShader(stage, pathToFile);
	}
	else {
		this->fragShader = GLSLShader(stage, pathToFile); 
	}
}

void VulkanObject::AddObject(std::string pathToFile)
{
	this->LoadObjFile(pathToFile); 
}

std::vector<Vertex> VulkanObject::GetVerticies()
{
	return this->vertexList;
}

std::vector<uint32_t> VulkanObject::GetIndicies()
{
	return this->indicies; 
}

std::vector<Shader> VulkanObject::GetShaders()
{
	std::vector<Shader> shaderList; 

	shaderList.push_back(this->vertexShader); 
	shaderList.push_back(this->fragShader); 

	return shaderList; 
}

Shader VulkanObject::GetVertShader()
{
	return this->vertexShader; 
}

Shader VulkanObject::GetFragShader()
{
	return this->fragShader; 
}

void VulkanObject::AddTexture(std::string pathToTexture)
{
	this->pathToTexture = pathToTexture; 
}

void VulkanObject::LoadObjFile(std::string pathToFile)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, pathToFile.c_str())) {
		throw std::runtime_error(warn + err);
	}

	//need to scale object so that it fits on screen
	
	float maxVal = 0; 
	std::unordered_map<Vertex, uint32_t> uniqueVerticies{}; 

	//combine all attributes into a single object 
	for (const auto& shape : shapes) {
		//tinyobj ensures three verticies per triangle  -- assuming unique verticies 
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
				//must multiply index by 3 due to object being type float rather than glm::vec3 
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			//TODO: scaling very bad...switch to method of moving camera 
			if (vertex.pos.x > maxVal) {
				maxVal = vertex.pos.x; 
			}
			if (vertex.pos.y > maxVal) {
				maxVal = vertex.pos.y; 
			}
			if (vertex.pos.z > maxVal) {
				maxVal = vertex.pos.z; 
			}

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				attrib.texcoords[2 * index.texcoord_index + 1]
			};

			if (uniqueVerticies.count(vertex) == 0) {
				uniqueVerticies[vertex] = static_cast<uint32_t>(vertexList.size());
				vertexList.push_back(vertex); 
			}

			this->indicies.push_back(uniqueVerticies[vertex]);
		}
	}

	//scale up factor 
	maxVal *= 0.5; 

	//SAME AS ABOVE 
	for (auto& vert : this->vertexList) {
		vert.pos.x /= maxVal; 
		vert.pos.y /= maxVal; 
		vert.pos.z /= maxVal; 
		vert.pos.z -= 0.7; 
	}
}

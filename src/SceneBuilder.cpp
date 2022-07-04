#include "SceneBuilder.hpp"

namespace star {
#pragma region GameObjects
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setPath(const std::string& path) {
		this->path = std::make_unique<std::string>(path); 
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setPosition(const glm::vec3 position) {
		this->position = position;
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setScale(const glm::vec3 scale) {
		this->scale = scale;
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setVertShader(const common::Handle& vertShader) {
		this->vertShader = vertShader;
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setFragShader(const common::Handle& fragShader) {
		this->fragShader = fragShader;
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setVerticies(const std::vector<glm::vec3>& verticies) {
		this->verticies = std::make_unique<std::vector<common::Vertex>>(verticies.size());

		for (auto& vert : verticies) {
			this->verticies->push_back(common::Vertex{ vert });
		}
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setIndicies(const std::vector<uint32_t>& indicies) {
		this->indicies = std::make_unique<std::vector<uint32_t>>(indicies);
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setTexture(const common::Handle& texture) {
		this->texture = texture;
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setMaterial(common::Material& material) {
		this->material = &material;
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::loadMaterials(const std::string& baseMaterialPath) {
		assert(baseMaterialPath == std::string() && this->path && "Called to load materials without proper base path available (.obj path OR provided materialPath Arguemnt)"); 

		this->loadFromDisk = true;
		this->baseMaterialPath = baseMaterialPath == std::string() ? std::make_unique<std::string>(common::FileHelpers::GetBaseFileDirectory(*this->path)) : std::make_unique<std::string>(baseMaterialPath);
		return *this; 
	}
	common::Handle SceneBuilder::GameObjects::Builder::build() {
		if (!this->verticies && !this->indicies && this->path) {
			return this->sceneBuilder.addObject(*this->path, this->position, this->scale, this->material, this->vertShader, this->fragShader, this->loadFromDisk, this->baseMaterialPath);  
		}
		else if (this->verticies && this->verticies->size() != 0 && this->indicies && this->indicies->size() != 0) {

			//return this->sceneBuilder.add(std::move(this->verticies), std::move(this->indicies), this->position, this->scale, this->material, this->texture, this->vertShader, this->fragShader);
		}
		throw std::runtime_error("Invalid parameters provided to complete build of object");
	}
	common::GameObject& SceneBuilder::GameObjects::Builder::buildGet() {
		if (!this->verticies && !this->indicies && this->path) {
			common::Handle newHandle = this->sceneBuilder.addObject(*this->path, this->position, this->scale, this->material, this->vertShader, this->fragShader, this->loadFromDisk, this->baseMaterialPath);
			return this->sceneBuilder.getObject(newHandle); 
		}
		else if (this->verticies && this->verticies->size() != 0 && this->indicies && this->indicies->size() != 0) {

			//return this->sceneBuilder.add(std::move(this->verticies), std::move(this->indicies), this->position, this->scale, this->material, this->texture, this->vertShader, this->fragShader);
		}
		throw std::runtime_error("Invalid parameters provided to complete build of object");
	}
#pragma endregion

	/* Lights */

	/* Material */

	SceneBuilder::Material::Builder& SceneBuilder::Material::Builder::setSurfaceColor(const glm::vec4& surfaceColor) {
		this->surfaceColor = surfaceColor;
		return *this; 
	}
	SceneBuilder::Material::Builder& SceneBuilder::Material::Builder::setHighlightColor(const glm::vec4& highlightColor)
	{
		this->highlightColor = highlightColor; 
		return *this; 
	}
	SceneBuilder::Material::Builder& SceneBuilder::Material::Builder::setShinyCoefficient(const int& shinyCoefficient) {
		this->shinyCoefficient = shinyCoefficient; 
		return *this; 
	}
	common::Handle SceneBuilder::Material::Builder::build() {
		return this->sceneBuilder.addMaterial(this->surfaceColor, this->highlightColor, this->shinyCoefficient);
	}
	common::Material& SceneBuilder::Material::Builder::buildGet() {
		common::Handle newHandle = this->sceneBuilder.addMaterial(this->surfaceColor, this->highlightColor, this->shinyCoefficient); 
		return this->sceneBuilder.getMaterial(newHandle); 
	}
	
	/* Mesh */

	/* Scene Builder */

	common::Handle SceneBuilder::addObject(const std::string& pathToFile, glm::vec3& position, glm::vec3& scaleAmt,
		common::Material* material, common::Handle& vertShader,
		common::Handle& fragShader, bool loadMaterials, 
		const std::unique_ptr<std::string>& materialBasePath) {
		std::unique_ptr<common::GameObject> object;
		std::string baseDir = !materialBasePath ? common::FileHelpers::GetBaseFileDirectory(pathToFile) : *materialBasePath;

		//apply default material if needed
		if (material == nullptr) {
			material = this->defaultMaterial; 
		}

		/* Load Object From File */
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, pathToFile.c_str(), baseDir.c_str())) {
			throw std::runtime_error(warn + err);
		}

		float maxVal = 0;
		size_t counter = 0; 
		size_t shapeCounter = 0; 
		size_t materialIndex = 0;
		common::Vertex* currVertex;
		std::unique_ptr<std::vector<common::Vertex>> verticies; 
		std::unique_ptr<std::vector<uint32_t>> indicies; 
		std::vector<common::Handle> modelTextures(materials.size());
		std::vector<std::unique_ptr<common::Mesh>> meshes(shapes.size());
		tinyobj::material_t* currMaterial = nullptr;

		if (loadMaterials) {
			//create needed materials
			for (size_t i = 0; i < materials.size(); i++) {
				currMaterial = &materials.at(i);

				modelTextures.at(i) = this->textureManager.add(baseDir + currMaterial->diffuse_texname);
			}
		}
		//need to scale object so that it fits on screen
		//combine all attributes into a single object 
		for (const auto& shape : shapes) {
			//tinyobj ensures three verticies per triangle  -- assuming unique verticies 
			counter = 0; 
			verticies = std::make_unique<std::vector<common::Vertex>>(shape.mesh.indices.size());
			indicies = std::make_unique<std::vector<uint32_t>>(); 

			for (const auto& index : shape.mesh.indices) {

				common::Vertex vertex{};
				if (index.vertex_index >= 0) {
					vertex.pos = {
						//must multiply index by 3 due to object being type float rather than glm::vec3 
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

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
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]			//image needs to be flipped where 0 is the top (-1.0)
				};

				verticies->at(counter) = vertex; 
				indicies->push_back(indicies->size()); 
				counter++; 
			}

			if (loadMaterials) {
				materialIndex = shape.mesh.material_ids.at(shapeCounter);
				if (materialIndex != -1) {
					meshes.at(shapeCounter) = std::move(common::Mesh::Builder()
						.setIndicies(std::move(indicies))
						.setVerticies(std::move(verticies))
						.setTexture(modelTextures.at(materialIndex))
						.build());
						//.setMaterial(common::Handle{
						//	this->materialManager.size() + materialIndex,
						//	common::Handle_Type::material })
						//	.build());
				}
				else {
					meshes.at(shapeCounter) = std::move(common::Mesh::Builder()
						.setIndicies(std::move(indicies))
						.setVerticies(std::move(verticies))
						.build());
				}
			}

			shapeCounter++; 
		}

		std::cout << "Loaded: " << pathToFile << std::endl;

		//TODO: give texture handle to material 
		return this->objectManager.add(std::make_unique<common::GameObject>(position, scaleAmt, material, vertShader, fragShader, std::move(meshes)));

		//common::Handle newHandle = this->objectManager.Add(pathToFile, material, position, scaleAmt, vertShader, fragShader); 
		//newHandle.type = common::Handle_Type::object; 
		//return newHandle; 
	}

	common::Handle SceneBuilder::addMaterial(const glm::vec4& surfaceColor, const glm::vec4& hightlightColor, const int& shinyCoefficient) {
		return this->materialManager.add(surfaceColor, hightlightColor, shinyCoefficient); 
	}

	common::GameObject& SceneBuilder::getObject(const common::Handle& handle) {
		if (handle.type & common::Handle_Type::object) {
			return this->objectManager.get(handle); 
		}

		throw std::runtime_error("Requested handle is not a game object handle");
	}

	common::Material& SceneBuilder::getMaterial(const common::Handle& handle) {
		if (handle.type & common::Handle_Type::material) {
			return this->materialManager.get(handle); 
		}
		
		throw std::runtime_error("Requested handle is not a material handle");
	}
}
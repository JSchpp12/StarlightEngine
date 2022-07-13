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
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setMaterial(common::Handle& materialHandle) {
		this->materialHandle = &materialHandle;
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setMaterialFilePath(const std::string& path) {
		this->materialFilePath = std::make_unique<std::string>(path); 
		return *this; 
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setTextureDirectory(const std::string& path) {
		this->textureDirectory = std::make_unique<std::string>(path);
		return *this; 
	}
	common::Handle SceneBuilder::GameObjects::Builder::build(bool loadMaterials) {
		if (!this->verticies && !this->indicies && this->path) {
			return this->sceneBuilder.addObject(*this->path, this->position, this->scale, this->materialHandle, this->vertShader, this->fragShader, loadMaterials, this->materialFilePath.get(), this->textureDirectory.get());
		}
		else if (this->verticies && this->verticies->size() != 0 && this->indicies && this->indicies->size() != 0) {

			//return this->sceneBuilder.add(std::move(this->verticies), std::move(this->indicies), this->position, this->scale, this->material, this->texture, this->vertShader, this->fragShader);
		}
		throw std::runtime_error("Invalid parameters provided to complete build of object");
	}
	common::GameObject& SceneBuilder::GameObjects::Builder::buildGet(bool loadMaterials) {
		if (!this->verticies && !this->indicies && this->path) {
			common::Handle newHandle = this->sceneBuilder.addObject(*this->path, this->position, this->scale, this->materialHandle, this->vertShader, this->fragShader, loadMaterials, this->materialFilePath.get(), this->textureDirectory.get());
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
	SceneBuilder::Material::Builder& SceneBuilder::Material::Builder::setAmbient(const glm::vec4& ambient) {
		this->ambient = ambient; 
		return *this; 
	}
	SceneBuilder::Material::Builder& SceneBuilder::Material::Builder::setDiffuse(const glm::vec4& diffuse) {
		this->diffuse = diffuse; 
		return *this; 
	}
	SceneBuilder::Material::Builder& SceneBuilder::Material::Builder::setSpecular(const glm::vec4& specular) {
		this->specular = specular; 
		return *this; 
	}
	SceneBuilder::Material::Builder& SceneBuilder::Material::Builder::setShinyCoefficient(const int& shinyCoefficient) {
		this->shinyCoefficient = shinyCoefficient; 
		return *this; 
	}
	SceneBuilder::Material::Builder& SceneBuilder::Material::Builder::setTexture(common::Handle texture) {
		this->texture = texture; 
		return *this;
	}
	common::Handle SceneBuilder::Material::Builder::build() {
		return this->sceneBuilder.addMaterial(this->surfaceColor, this->highlightColor, this->diffuse,
			this->ambient, this->diffuse, this->shinyCoefficient,&this->texture);
	}
	common::Material& SceneBuilder::Material::Builder::buildGet() {
		common::Handle newHandle = this->sceneBuilder.addMaterial(this->surfaceColor, this->highlightColor, this->ambient, this->diffuse,
			this->specular, this->shinyCoefficient, 
			&this->texture); 
		return this->sceneBuilder.getMaterial(newHandle); 
	}
	
	/* Mesh */

	/* Scene Builder */

	common::Handle SceneBuilder::addObject(const std::string& pathToFile, glm::vec3& position, glm::vec3& scaleAmt,
		common::Handle* materialHandle, common::Handle& vertShader,
		common::Handle& fragShader, bool loadMaterials, 
		std::string* materialFilePath, std::string* textureDir) {
		std::unique_ptr<common::GameObject> object;
		std::string materialFile = !materialFilePath ? common::FileHelpers::GetBaseFileDirectory(pathToFile) : *materialFilePath;
		std::string texturePath = textureDir != nullptr ? *textureDir : common::FileHelpers::GetBaseFileDirectory(pathToFile);
		common::Handle selectedMaterial; 

		if (materialHandle == nullptr) {
			selectedMaterial = this->materialManager.getDefaultHandle();
		}
		else {
			selectedMaterial = *materialHandle;
		}

		/* Load Object From File */
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, pathToFile.c_str(), materialFile.c_str(), true)) {
			throw std::runtime_error(warn + err);
		}
		if (err != "") {
			std::cout << "An error occurred while loading obj file" << std::endl;
			std::cout << err << std::endl;
			std::cout << "Loading will continue..." << std::endl;
		}

		float maxVal = 0;
		size_t counter = 0;
		size_t shapeCounter = 0; 
		size_t materialIndex = 0;
		size_t triangleCounter = 0;
		size_t threeCounter = 0; 

		common::Vertex* currVertex = nullptr;
		common::Handle loadMaterialTexture; 
		std::unique_ptr<std::vector<common::Vertex>> verticies; 
		std::unique_ptr<std::vector<uint32_t>> indicies; 
		std::unique_ptr<std::vector<std::pair<unsigned int, unsigned int>>> sortedIds;
		std::vector<std::unique_ptr<common::Mesh>> meshes(shapes.size());
		std::vector<common::Handle> objectMaterialHandles;
		std::vector<std::reference_wrapper<common::Material>> objectMaterials; 
		std::unique_ptr<std::vector<common::Triangle>> triangles; 
		tinyobj::material_t* currMaterial = nullptr;
		std::unique_ptr<common::Material> objectMaterial; 

		if (loadMaterials) {
			//create needed materials
			for (size_t i = 0; i < materials.size(); i++) {
				currMaterial = &materials.at(i);
				loadMaterialTexture = this->textureManager.add(texturePath + common::FileHelpers::GetFileNameWithExtension(currMaterial->diffuse_texname));
				objectMaterialHandles.push_back(Material::Builder(*this)
					.setTexture(loadMaterialTexture)
					.setDiffuse(glm::vec4{
						currMaterial->diffuse[0], 
						currMaterial->diffuse[1], 
						currMaterial->diffuse[2], 
						1.0f })
					.setSpecular(glm::vec4{
						currMaterial->specular[0], 
						currMaterial->specular[1], 
						currMaterial->specular[2], 
						1.0f })
					.setShinyCoefficient(currMaterial->shininess)
					.build());
				//record material to avoid multiple fetches
				objectMaterials.push_back(this->materialManager.get(objectMaterialHandles.at(i))); 
			}
		}


		//need to scale object so that it fits on screen
		//combine all attributes into a single object 
		std::array<common::Vertex, 3> triangleVerticies; 
		int dIndex = 0; 
		for (const auto& shape : shapes) {
			triangleCounter = 0; 
			threeCounter = 0; 
			counter = 0;

			//tinyobj ensures three verticies per triangle  -- assuming unique verticies 
			verticies = std::make_unique<std::vector<common::Vertex>>(shape.mesh.indices.size());
			const std::vector<tinyobj::index_t>& indicies = shape.mesh.indices;
			triangles = std::make_unique<std::vector<common::Triangle>>(shape.mesh.material_ids.size()); 

			counter = 0;
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
				counter++;
			}

			for (size_t faceIndex = 0; faceIndex < shape.mesh.material_ids.size(); faceIndex++) {
				for (int i = 0; i < 3; i++) {
					dIndex = (3 * faceIndex) + i; 
					triangleVerticies[i].pos = {
						attrib.vertices[3 * indicies[dIndex].vertex_index + 0],
						attrib.vertices[3 * indicies[dIndex].vertex_index + 1],
						attrib.vertices[3 * indicies[dIndex].vertex_index + 2],
					};
					triangleVerticies[i].color = {
						attrib.colors[3 * indicies[dIndex].vertex_index + 0],
						attrib.colors[3 * indicies[dIndex].vertex_index + 1],
						attrib.colors[3 * indicies[dIndex].vertex_index + 2],
					};
					triangleVerticies[i].normal = {
						attrib.normals[3 * indicies[dIndex].vertex_index + 0],
						attrib.normals[3 * indicies[dIndex].vertex_index + 1],
						attrib.normals[3 * indicies[dIndex].vertex_index + 2],
					};
					triangleVerticies[i].texCoord = {
						attrib.texcoords[2 * indicies[dIndex].texcoord_index + 0],
						1.0f - attrib.texcoords[2 * indicies[dIndex].texcoord_index + 1 ]
					};
					if (loadMaterials) {
						triangleVerticies[i].matAmbient = objectMaterials.at(shape.mesh.material_ids.at(faceIndex)).get().ambient;
						triangleVerticies[i].matDiffuse = objectMaterials.at(shape.mesh.material_ids.at(faceIndex)).get().diffuse;
						triangleVerticies[i].matSpecular = objectMaterials.at(shape.mesh.material_ids.at(faceIndex)).get().specular;
						triangleVerticies[i].matShininess = objectMaterials.at(shape.mesh.material_ids.at(faceIndex)).get().shinyCoefficient;
					}
				}

				triangles->at(faceIndex) = common::Triangle(triangleVerticies); 
			}

			if (loadMaterials && shape.mesh.material_ids.at(shapeCounter) != -1) {
				//apply material from files to mesh -- will ignore passed values 
				meshes.at(shapeCounter) = std::move(common::Mesh::Builder()
					.setTriangles(std::move(triangles))
					.setMaterial(objectMaterialHandles.at(shape.mesh.material_ids[0]))
					.build());
			}
			else {
				meshes.at(shapeCounter) = std::move(common::Mesh::Builder()
					.setTriangles(std::move(triangles))
					.setMaterial(selectedMaterial)
					.build());
			}
			shapeCounter++; 
		}

		std::cout << "Loaded: " << pathToFile << std::endl;

		//TODO: give texture handle to material 
		return this->objectManager.addUniqueResource(std::make_unique<common::GameObject>(position, scaleAmt, vertShader, fragShader, std::move(meshes)));
	}

	common::Handle SceneBuilder::addMaterial(const glm::vec4& surfaceColor, const glm::vec4& hightlightColor, const glm::vec4& ambient, 
		const glm::vec4& diffuse, const glm::vec4& specular, 
		const int& shinyCoefficient, common::Handle* texture) {
		if (texture != nullptr) {
			return this->materialManager.add(surfaceColor, hightlightColor, ambient, diffuse, specular, shinyCoefficient, *texture);
		}
		return this->materialManager.add(surfaceColor, hightlightColor, ambient, diffuse, specular, shinyCoefficient); 
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
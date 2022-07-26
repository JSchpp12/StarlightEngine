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
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setColor(const glm::vec4& color) {
		this->color = &color; 
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
		if (!this->matOverride) {
			this->matOverride = std::make_unique<OverrideMaterialProperties>(); 
		}
		this->matOverride->baseColorTexture = &texture; 
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setMaterial(common::Handle materialHandle) {
		this->materialHandle = &materialHandle;
		return *this;
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::overrideAmbient(const glm::vec3& ambient) {
		if (!this->matOverride) {
			this->matOverride = std::make_unique<OverrideMaterialProperties>(); 
		}
		this->matOverride->ambient = &ambient; 
		return *this; 
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::overrideDiffuse(const glm::vec3& diffuse) {
		if (!this->matOverride) {
			this->matOverride = std::make_unique<OverrideMaterialProperties>(); 
		}
		this->matOverride->diffuse = &diffuse; 
		return *this; 
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::overrideSpecular(const glm::vec3& specular) {
		if (!this->matOverride) {
			this->matOverride = std::make_unique<OverrideMaterialProperties>(); 
		}
		this->matOverride->specular = &specular; 
		return *this; 
	}
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::overrideShiny(const float& shiny) {
		if (!this->matOverride) {
			this->matOverride = std::make_unique<OverrideMaterialProperties>(); 
		}
		this->matOverride->shiny = &shiny; 
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
			return this->sceneBuilder.addObject(*this->path, this->position, this->scale, this->materialHandle, this->vertShader, this->fragShader, loadMaterials, this->materialFilePath.get(), this->textureDirectory.get(), this->color, this->matOverride.get());
		}
		else if (this->verticies && this->verticies->size() != 0 && this->indicies && this->indicies->size() != 0) {

			//return this->sceneBuilder.add(std::move(this->verticies), std::move(this->indicies), this->position, this->scale, this->material, this->texture, this->vertShader, this->fragShader);
		}
		throw std::runtime_error("Invalid parameters provided to complete build of object");
	}
#pragma endregion

	/* Lights */
	SceneBuilder::Lights::Builder& SceneBuilder::Lights::Builder::setType(const common::Type::Light& type) {
		this->type = &type;
		return *this; 
	}
	SceneBuilder::Lights::Builder& SceneBuilder::Lights::Builder::setLinkedObject(const common::Handle& linkedObject) {
		this->linkedHandle = &linkedObject; 
		return *this; 
	}
	SceneBuilder::Lights::Builder& SceneBuilder::Lights::Builder::setPosition(const glm::vec3& position) {
		//TODO: need to handle which object (light/Linked GO) will be used for the position 
		this->position = &position; 
		return *this; 
	}
	SceneBuilder::Lights::Builder& SceneBuilder::Lights::Builder::setAmbient(const glm::vec4& ambient) {
		this->ambient = &ambient; 
		return *this; 
	}
	SceneBuilder::Lights::Builder& SceneBuilder::Lights::Builder::setDiffuse(const glm::vec4& diffuse) {
		this->diffuse = &diffuse; 
		return *this; 
	}
	SceneBuilder::Lights::Builder& SceneBuilder::Lights::Builder::setSpecular(const glm::vec4& specular)
	{
		this->specular = &specular; 
		return *this; 
	}	
	common::Handle SceneBuilder::Lights::Builder::build() {
		assert(this->position && this->type && "A light must have a position and type"); 

		if (this->linkedHandle != nullptr) {
			return this->sceneBuilder.addLight(*this->type, *this->position, *this->linkedHandle, this->ambient, this->diffuse, this->specular);
		}
		return this->sceneBuilder.addLight(*this->type, *this->position, this->ambient, this->diffuse, this->specular);
	}
	
	/* Material */

	SceneBuilder::Materials::Builder& SceneBuilder::Materials::Builder::setSurfaceColor(const glm::vec4& surfaceColor) {
		this->surfaceColor = surfaceColor;
		return *this; 
	}
	SceneBuilder::Materials::Builder& SceneBuilder::Materials::Builder::setHighlightColor(const glm::vec4& highlightColor)
	{
		this->highlightColor = highlightColor; 
		return *this; 
	}
	SceneBuilder::Materials::Builder& SceneBuilder::Materials::Builder::setAmbient(const glm::vec4& ambient) {
		this->ambient = ambient; 
		return *this; 
	}
	SceneBuilder::Materials::Builder& SceneBuilder::Materials::Builder::setDiffuse(const glm::vec4& diffuse) {
		this->diffuse = diffuse; 
		return *this; 
	}
	SceneBuilder::Materials::Builder& SceneBuilder::Materials::Builder::setSpecular(const glm::vec4& specular) {
		this->specular = specular; 
		return *this; 
	}
	SceneBuilder::Materials::Builder& SceneBuilder::Materials::Builder::setShinyCoefficient(const int& shinyCoefficient) {
		this->shinyCoefficient = shinyCoefficient; 
		return *this; 
	}
	SceneBuilder::Materials::Builder& SceneBuilder::Materials::Builder::setTexture(common::Handle texture) {
		this->texture = texture; 
		return *this;
	}
	SceneBuilder::Materials::Builder& SceneBuilder::Materials::Builder::setBumpMap(const common::Handle& bumpHandle) {
		this->bumpMap = bumpHandle; 
		return *this; 
	}
	common::Handle SceneBuilder::Materials::Builder::build() {
		return this->sceneBuilder.addMaterial(this->surfaceColor, this->highlightColor, this->diffuse,
			this->diffuse, this->specular, this->shinyCoefficient,&this->texture, &this->bumpMap);
	}
	
	/* Mesh */

	/* Scene Builder */

	common::GameObject& SceneBuilder::entity(const common::Handle& handle) {
		if (handle.type == common::Handle_Type::object) {
			return this->objectManager.resource(handle);
		}

		throw std::runtime_error("Requested handle is not a game object handle");
	}

	common::Material& SceneBuilder::getMaterial(const common::Handle& handle) {
		if (handle.type & common::Handle_Type::material) {
			return this->materialManager.resource(handle);
		}

		throw std::runtime_error("Requested handle is not a material handle");
	}

	common::Handle SceneBuilder::addObject(const std::string& pathToFile, glm::vec3& position, glm::vec3& scaleAmt,
		common::Handle* materialHandle, common::Handle& vertShader,
		common::Handle& fragShader, bool loadMaterials, 
		std::string* materialFilePath, std::string* textureDir, 
		const glm::vec4* overrideColor, const GameObjects::Builder::OverrideMaterialProperties* matPropOverride) {
		std::unique_ptr<common::GameObject> object;
		std::string materialFile = !materialFilePath ? common::FileHelpers::GetBaseFileDirectory(pathToFile) : *materialFilePath;
		std::string texturePath = textureDir != nullptr ? *textureDir : common::FileHelpers::GetBaseFileDirectory(pathToFile);
		common::Handle selectedMaterial; 

		if (materialHandle == nullptr) {
			selectedMaterial = common::Handle::getDefault();
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
		if (warn != "") {
			std::cout << "An error occurred while loading obj file" << std::endl;
			std::cout << warn << std::endl;
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

		if (loadMaterials && materials.size() > 0) {
			//create needed materials
			for (size_t i = 0; i < materials.size(); i++) {
				currMaterial = &materials.at(i);
				auto& builder = Materials::Builder(*this)
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
					.setShinyCoefficient(currMaterial->shininess); 
				//check if need to override texture 
				if (matPropOverride != nullptr && matPropOverride->baseColorTexture != nullptr) {
					builder.setTexture(*matPropOverride->baseColorTexture);
				}else if (currMaterial->diffuse_texname != "") {
					loadMaterialTexture = this->textureManager.addResource(texturePath + common::FileHelpers::GetFileNameWithExtension(currMaterial->diffuse_texname));
					builder.setTexture(loadMaterialTexture); 
				}

				//apply maps 
				if (currMaterial->bump_texname != "") {
					auto bumpTexture = this->textureManager.addResource(texturePath + common::FileHelpers::GetFileNameWithExtension(currMaterial->bump_texname));
					builder.setBumpMap(bumpTexture);
				}
				else {
					builder.setBumpMap(common::Handle::getDefault());
				}

				objectMaterialHandles.push_back(builder.build());
				//record material to avoid multiple fetches
				objectMaterials.push_back(this->materialManager.resource(objectMaterialHandles.at(i))); 
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

			for (size_t faceIndex = 0; faceIndex < shape.mesh.material_ids.size(); faceIndex++) {
				for (int i = 0; i < 3; i++) {
					dIndex = (3 * faceIndex) + i; 
					triangleVerticies[i].pos = {
						attrib.vertices[3 * indicies[dIndex].vertex_index + 0],
						attrib.vertices[3 * indicies[dIndex].vertex_index + 1],
						attrib.vertices[3 * indicies[dIndex].vertex_index + 2],
					};

					//check for color override
					if (!overrideColor) {
						triangleVerticies[i].color = {
							attrib.colors[3 * indicies[dIndex].vertex_index + 0],
							attrib.colors[3 * indicies[dIndex].vertex_index + 1],
							attrib.colors[3 * indicies[dIndex].vertex_index + 2],
						};
					}
					else {
						triangleVerticies[i].color = {
							overrideColor->r,
							overrideColor->g,
							overrideColor->b
						};
					}

					if (attrib.normals.size() > 0) {
						triangleVerticies[i].normal = {
							attrib.normals[3 * indicies[dIndex].normal_index + 0],
							attrib.normals[3 * indicies[dIndex].normal_index + 1],
							attrib.normals[3 * indicies[dIndex].normal_index + 2],
						};
					}

					triangleVerticies[i].texCoord = {
						attrib.texcoords[2 * indicies[dIndex].texcoord_index + 0],
						1.0f - attrib.texcoords[2 * indicies[dIndex].texcoord_index + 1 ]
					};
					if (loadMaterials && shape.mesh.material_ids.at(faceIndex) != -1) {
						//use the overridden material if provided, otherwise use the prop from mtl file
						triangleVerticies[i].matAmbient = (matPropOverride != nullptr && matPropOverride->ambient != nullptr) ? *matPropOverride->ambient		 : objectMaterials.at(shape.mesh.material_ids.at(faceIndex)).get().ambient;
						triangleVerticies[i].matDiffuse = (matPropOverride != nullptr && matPropOverride->diffuse != nullptr) ? *matPropOverride->diffuse		 : objectMaterials.at(shape.mesh.material_ids.at(faceIndex)).get().diffuse;
						triangleVerticies[i].matSpecular = (matPropOverride != nullptr && matPropOverride->specular != nullptr) ? *matPropOverride->specular	 : objectMaterials.at(shape.mesh.material_ids.at(faceIndex)).get().specular;
						triangleVerticies[i].matShininess = (matPropOverride != nullptr && matPropOverride->shiny != nullptr) ? *matPropOverride->shiny			 : objectMaterials.at(shape.mesh.material_ids.at(faceIndex)).get().shinyCoefficient;
					}
					else {
						//use either the overriden material property or the passed material
						triangleVerticies[i].matAmbient = (matPropOverride != nullptr && matPropOverride->ambient != nullptr)	 ? *matPropOverride->ambient	: this->materialManager.resource(selectedMaterial).ambient;
						triangleVerticies[i].matDiffuse = (matPropOverride != nullptr && matPropOverride->diffuse != nullptr)	 ? *matPropOverride->diffuse	: this->materialManager.resource(selectedMaterial).diffuse;
						triangleVerticies[i].matSpecular = (matPropOverride != nullptr && matPropOverride->specular != nullptr)	 ? *matPropOverride->specular	: this->materialManager.resource(selectedMaterial).specular;
						triangleVerticies[i].matShininess = (matPropOverride != nullptr && matPropOverride->shiny != nullptr)	 ? *matPropOverride->shiny		: this->materialManager.resource(selectedMaterial).shinyCoefficient;
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
		return this->objectManager.addResource(std::make_unique<common::GameObject>(position, scaleAmt, vertShader, fragShader, std::move(meshes)));
	}

	common::Handle SceneBuilder::addMaterial(const glm::vec4& surfaceColor, const glm::vec4& hightlightColor, const glm::vec4& ambient, 
		const glm::vec4& diffuse, const glm::vec4& specular, 
		const int& shinyCoefficient, common::Handle* texture, 
		common::Handle* bumpMap) {
		if (texture != nullptr && bumpMap != nullptr) {
			return this->materialManager.add(surfaceColor, hightlightColor, ambient, diffuse, specular, shinyCoefficient, *texture, *bumpMap);
		}
		return this->materialManager.add(surfaceColor, hightlightColor, ambient, diffuse, specular, shinyCoefficient); 
	}

	common::Handle SceneBuilder::addLight(const common::Type::Light& type, const glm::vec3& position, const common::Handle& linkedHandle, 
		const glm::vec4* ambient, const glm::vec4* diffuse, 
		const glm::vec4* specular) {
		common::GameObject& linkedObject = this->objectManager.resource(linkedHandle);
		linkedObject.setPosition(position);
		return this->lightManager.addResource(std::make_unique<common::Light>(type, position, linkedObject.getScale(), linkedHandle, linkedObject, ambient, diffuse, specular));
	}

	common::Handle SceneBuilder::addLight(const common::Type::Light& type, const glm::vec3& position, const glm::vec4* ambient,
		const glm::vec4* diffuse, const glm::vec4* specular) {
		return this->lightManager.addResource(std::make_unique<common::Light>(type, position, ambient, diffuse, specular));
	}
}
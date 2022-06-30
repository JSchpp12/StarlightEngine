#include "SceneBuilder.hpp"

namespace star {
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
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setMaterial(common::Material* material) {
		this->material = material;
		return *this;
	}
	common::Handle SceneBuilder::GameObjects::Builder::build() {
		if (!this->verticies && !this->indicies && this->path) {
			return this->sceneBuilder.addObject(*this->path, this->position, this->scale, this->material, this->texture, this->vertShader, this->fragShader);  
		}
		else if (this->verticies && this->verticies->size() != 0 && this->indicies && this->indicies->size() != 0) {

			//return this->sceneBuilder.add(std::move(this->verticies), std::move(this->indicies), this->position, this->scale, this->material, this->texture, this->vertShader, this->fragShader);
		}
		throw std::runtime_error("Invalid parameters provided to complete build of object");
	}
	common::GameObject* SceneBuilder::GameObjects::Builder::buildGet() {
		if (!this->verticies && !this->indicies && this->path) {
			common::Handle newHandle = this->sceneBuilder.addObject(*this->path, this->position, this->scale, this->material, this->texture, this->vertShader, this->fragShader);
			return this->sceneBuilder.getObject(newHandle); 
		}
		else if (this->verticies && this->verticies->size() != 0 && this->indicies && this->indicies->size() != 0) {

			//return this->sceneBuilder.add(std::move(this->verticies), std::move(this->indicies), this->position, this->scale, this->material, this->texture, this->vertShader, this->fragShader);
		}
		throw std::runtime_error("Invalid parameters provided to complete build of object");
	}

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
	common::Material* SceneBuilder::Material::Builder::buildGet() {
		common::Handle newHandle = this->sceneBuilder.addMaterial(this->surfaceColor, this->highlightColor, this->shinyCoefficient); 
		return this->sceneBuilder.getMaterial(newHandle); 
	}

	/* Scene Builder */

	common::Handle SceneBuilder::addObject(const std::string& pathToFile, glm::vec3& position, glm::vec3& scaleAmt, common::Material* material,
		common::Handle& texture, common::Handle& vertShader,
		common::Handle& fragShader) {

		//apply default material if needed
		if (material == nullptr) {
			material = this->defaultMaterial; 
		}

		common::Handle newHandle = this->objectManager.Add(pathToFile, material, position, scaleAmt, texture, vertShader, fragShader); 
		newHandle.type = common::Handle_Type::object; 
		return newHandle; 
	}

	common::Handle SceneBuilder::addMaterial(const glm::vec4& surfaceColor, const glm::vec4& hightlightColor, const int& shinyCoefficient) {
		return this->materialManager.add(surfaceColor, hightlightColor, shinyCoefficient); 
	}

	common::GameObject* SceneBuilder::getObject(const common::Handle& handle) {
		if (handle.type & common::Handle_Type::object) {
			return this->objectManager.get(handle); 
		}

		throw std::runtime_error("Requested handle is not a game object handle");
	}

	common::Material* SceneBuilder::getMaterial(const common::Handle& handle) {
		if (handle.type & common::Handle_Type::material) {
			return this->materialManager.get(handle); 
		}
		
		throw std::runtime_error("Requested handle is not a material handle");
	}
}
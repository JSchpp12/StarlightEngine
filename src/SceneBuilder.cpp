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
	SceneBuilder::GameObjects::Builder& SceneBuilder::GameObjects::Builder::setMaterial(const common::Material& material) {
		this->material = material;
		return *this;
	}
	common::Handle SceneBuilder::GameObjects::Builder::build() {
		if (!this->verticies && !this->indicies && this->path) {
			return this->sceneBuilder.add(*this->path, this->position, this->scale, this->texture, this->vertShader, this->fragShader);  
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

	//common::Handle SceneBuilder::Material::Builder::build() {
	//	
	//}

	/* Scene Builder */

	//common::Handle SceneBuilder::add(std::unique_ptr<common::Light> newLight) {
	//	return common::Handle{}; 
	//}

	common::Handle SceneBuilder::add(const std::string& pathToFile, glm::vec3& position, glm::vec3& scaleAmt,
		common::Handle& texture, common::Handle& vertShader,
		common::Handle& fragShader) {
		common::Handle newHandle = this->objectManager.Add(pathToFile, position, scaleAmt, texture, vertShader, fragShader); 
		newHandle.type = common::Handle_Type::object; 
		return newHandle; 
	}

	common::GameObject* SceneBuilder::get(const common::Handle& handle) {
		if (handle.type & common::Handle_Type::object) {
			return this->objectManager.Get(handle); 
		}

		throw std::runtime_error("Requested handle is not a game object handle");
	}
}
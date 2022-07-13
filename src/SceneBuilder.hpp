#pragma once 
#include "SC/FileHelpers.h"
#include "SC/Light.hpp"
#include "SC/Handle.hpp"
#include "SC/GameObject.hpp"

#include "ObjectManager.hpp"
#include "MaterialManager.hpp"
#include "TextureManager.h"

#include <glm/glm.hpp>
#include <tiny_obj_loader.h>

#include <memory>
#include <string>
#include <vector>

namespace star {
	class SceneBuilder {
	public:
		class GameObjects {
		public:
			class Builder {
			public:
				Builder(SceneBuilder& sceneBuilder) : sceneBuilder(sceneBuilder) {};
				Builder& setPosition(const glm::vec3 position);
				Builder& setPath(const std::string& path);
				Builder& setScale(const glm::vec3 scale);
				Builder& setVertShader(const common::Handle& vertShader);
				Builder& setFragShader(const common::Handle& fragShader);
				Builder& setTexture(const common::Handle& texture);
				Builder& setVerticies(const std::vector<glm::vec3>& verticies);
				Builder& setIndicies(const std::vector<uint32_t>& indicies);
				Builder& setMaterial(common::Handle& materialHandle);
				Builder& setMaterialFilePath(const std::string& path); 
				Builder& setTextureDirectory(const std::string& path); 
				common::Handle build(bool loadMaterials = true);
				common::GameObject& buildGet(bool loadMaterials = true); 

			protected:

			private:
				SceneBuilder& sceneBuilder;
				bool loadFromDisk = true; 
				glm::vec3 scale = glm::vec3{ 1.0f, 1.0f, 1.0f };
				glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f };
				common::Handle vertShader = common::Handle{ 0 };
				common::Handle fragShader = common::Handle{ 1 };
				common::Handle texture = common::Handle{ 0 };
				common::Handle* materialHandle = nullptr;
				std::unique_ptr<std::string> path;
				std::unique_ptr<std::string> materialFilePath;
				std::unique_ptr<std::string> textureDirectory;
				std::unique_ptr<std::vector<uint32_t>> indicies;
				std::unique_ptr<std::vector<common::Vertex>> verticies;
			};

		private:


		};

		class Lights {
		public:
			class Builder {
			public:
				Builder(SceneBuilder& sceneBuilder) : sceneBuilder(sceneBuilder) { }
				Builder& setType(const common::Light& type); 
				Builder& setColor(const glm::vec4& color); 
				Builder& setPosition(const glm::vec4& position); 
				//common::Handle build(); 

			private:
				SceneBuilder& sceneBuilder; 

			};
		private:


		};
		//TODO: change this to 'Materials'
		class Material {
		public:
			class Builder {
			public:
				Builder(SceneBuilder& sceneBuilder) : sceneBuilder(sceneBuilder), 
					diffuse(sceneBuilder.defaultMaterial->diffuse), 
					specular(sceneBuilder.defaultMaterial->specular) {}
				Builder& setSurfaceColor(const glm::vec4& surfaceColor);
				Builder& setHighlightColor(const glm::vec4& highlightColor);
				Builder& setAmbient(const glm::vec4& ambient);
				Builder& setDiffuse(const glm::vec4& diffuse); 
				Builder& setSpecular(const glm::vec4& specular); 
				Builder& setShinyCoefficient(const int& shinyCoefficient);
				Builder& setTexture(common::Handle texture); 
				common::Handle build();
				common::Material& buildGet(); 

			private:
				SceneBuilder& sceneBuilder;
				glm::vec4 surfaceColor = sceneBuilder.defaultMaterial->surfaceColor;
				glm::vec4 highlightColor = sceneBuilder.defaultMaterial->highlightColor; 
				glm::vec4 diffuse; 
				glm::vec4 specular; 
				glm::vec4 ambient; 
				int shinyCoefficient = sceneBuilder.defaultMaterial->shinyCoefficient;
				common::Handle texture; 
			};
		};

		SceneBuilder(core::ObjectManager& objectManager, core::MaterialManager& materialManager, core::TextureManager& textureManager) 
			: objectManager(objectManager), materialManager(materialManager), 
			defaultMaterial(materialManager.getDefault()), textureManager(textureManager) { }
		~SceneBuilder() = default;

		common::GameObject& getObject(const common::Handle& handle);
		common::Material& getMaterial(const common::Handle& handle);

	private:
		core::ObjectManager& objectManager; 
		core::MaterialManager& materialManager; 
		core::TextureManager& textureManager; 

		//defaults -- TODO: remove this in favor of each manager having its own default 
		common::Material* defaultMaterial = nullptr; 

		common::Handle addObject(const std::string& pathToFile, glm::vec3& position, glm::vec3& scaleAmt, 
			common::Handle* materialHandle, common::Handle& vertShader,
			common::Handle& fragShader, bool loadMaterials, 
			std::string* materialFilePath, std::string* textureDir);

		common::Handle addMaterial(const glm::vec4& surfaceColor, const glm::vec4& hightlightColor, const glm::vec4& ambient, 
			const glm::vec4& diffuse, const glm::vec4& specular,
			const int& shinyCoefficient, common::Handle* texture);
 
		friend class common::Mesh::Builder; 
		friend class GameObjects::Builder;
	};
}
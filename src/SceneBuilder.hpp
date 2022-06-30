#pragma once 

#include "SC/Light.hpp"
#include "SC/Handle.hpp"
#include "SC/GameObject.hpp"

#include "ObjectManager.hpp"
#include "MaterialManager.hpp"

#include <glm/glm.hpp>

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
				Builder& setMaterial(common::Material* material);
				common::Handle build();
				common::GameObject* buildGet(); 

			protected:

			private:
				SceneBuilder& sceneBuilder;
				glm::vec3 scale = glm::vec3{ 1.0f, 1.0f, 1.0f };
				glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f };
				common::Handle vertShader = common::Handle{ 0 };
				common::Handle fragShader = common::Handle{ 1 };
				common::Handle texture = common::Handle{ 0 };
				common::Material* material = nullptr;
				std::unique_ptr<std::string> path;
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
		class Material {
		public:
			class Builder {
			public:
				Builder(SceneBuilder& sceneBuilder) : sceneBuilder(sceneBuilder) {}
				Builder& setSurfaceColor(const glm::vec4& surfaceColor);
				Builder& setHighlightColor(const glm::vec4& highlightColor);
				Builder& setShinyCoefficient(const int& shinyCoefficient);
				common::Handle build();
				common::Material* buildGet(); 

			private:
				SceneBuilder& sceneBuilder;
				glm::vec4 surfaceColor = sceneBuilder.defaultMaterial->surfaceColor;
				glm::vec4 highlightColor = sceneBuilder.defaultMaterial->highlightColor; 
				int shinyCoefficient = sceneBuilder.defaultMaterial->shinyCoefficient;
			};
		};

		SceneBuilder(core::ObjectManager& objectManager, core::MaterialManager& materialManager)
			: objectManager(objectManager), materialManager(materialManager), defaultMaterial(materialManager.getDefault()) { }
		~SceneBuilder() = default;

		//common::Handle add(std::unique_ptr<common::Light> newLight);

		common::Handle addObject(const std::string& pathToFile, glm::vec3& position, glm::vec3& scaleAmt, common::Material* material,
			common::Handle& texture, common::Handle& vertShader,
			common::Handle& fragShader);

		common::Handle addMaterial(const glm::vec4& surfaceColor, const glm::vec4& hightlightColor, const int& shinyCoefficient);

		common::GameObject* getObject(const common::Handle& handle); 

		common::Material* getMaterial(const common::Handle& handle); 

	private:
		core::ObjectManager& objectManager; 
		core::MaterialManager& materialManager; 

		//defaults 
		common::Material* defaultMaterial = nullptr; 

		friend class Material::Builder; 
		friend class GameObjects::Builder;
	};
}
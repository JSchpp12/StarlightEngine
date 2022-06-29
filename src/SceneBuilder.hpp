#pragma once 

#include "SC/Light.hpp"
#include "SC/Handle.hpp"
#include "SC/GameObject.hpp"

#include "ObjectManager.hpp"

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
				Builder& setMaterial(const common::Material& material);
				common::Handle build();

			protected:

			private:
				SceneBuilder& sceneBuilder;
				glm::vec3 scale = glm::vec3{ 1.0f, 1.0f, 1.0f };
				glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f };
				common::Handle vertShader = common::Handle{ 0 };
				common::Handle fragShader = common::Handle{ 1 };
				common::Handle texture = common::Handle{ 0 };
				common::Material material;
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
				Builder& setType(); 
				Builder& setColor(); 
				Builder& setPosition(); 
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
				common::Handle build();

			private:
				SceneBuilder& sceneBuilder;
				glm::vec4 surfaceColor;
				glm::vec4 highlightColor;
			};
		};

		SceneBuilder(core::ObjectManager& objectManager) : objectManager(objectManager) { }
		~SceneBuilder() = default;

		//common::Handle add(std::unique_ptr<common::Light> newLight);

		common::Handle add(const std::string& pathToFile, glm::vec3& position, glm::vec3& scaleAmt,
			common::Handle& texture, common::Handle& vertShader,
			common::Handle& fragShader);

		common::GameObject* get(const common::Handle& handle); 

	private:
		core::ObjectManager& objectManager; 

	};
}
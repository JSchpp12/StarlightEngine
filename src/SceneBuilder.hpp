#pragma once 
#include "SC/FileHelpers.h"
#include "SC/Light.hpp"
#include "SC/Handle.hpp"
#include "SC/GameObject.hpp"
#include "SC/Triangle.hpp"
#include "SC/Vertex.hpp"

#include "ObjectManager.hpp"
#include "MaterialManager.hpp"
#include "LightManager.hpp"
#include "TextureManager.hpp"
#include "MapManager.hpp"

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
				struct OverrideMaterialProperties{
					const common::Handle* baseColorTexture = nullptr; 
					const glm::vec3* ambient = nullptr; 
					const glm::vec3* diffuse = nullptr;
					const glm::vec3* specular = nullptr;
					const float* shiny = nullptr; 
				};

				Builder(SceneBuilder& sceneBuilder) : sceneBuilder(sceneBuilder) {
					vertShader.shaderStage = common::Shader_Stage::vertex; 
					fragShader.shaderStage = common::Shader_Stage::fragment; 
				};
				Builder& setPosition(const glm::vec3 position);
				//override vertex colors from file with a predefined one
				Builder& setColor(const glm::vec4& color); 
				Builder& setPath(const std::string& path);
				Builder& setScale(const glm::vec3 scale);
				Builder& setVertShader(const common::Handle& vertShader);
				Builder& setFragShader(const common::Handle& fragShader);
				Builder& setTexture(const common::Handle& texture);
				Builder& setVerticies(const std::vector<glm::vec3>& verticies);
				Builder& setIndicies(const std::vector<uint32_t>& indicies);
				Builder& setMaterial(common::Handle materialHandle);
				Builder& overrideAmbient(const glm::vec3& ambient);
				Builder& overrideDiffuse(const glm::vec3& diffuse); 
				Builder& overrideSpecular(const glm::vec3& specular); 
				Builder& overrideShiny(const float& shiny); 
				Builder& setMaterialFilePath(const std::string& path); 
				Builder& setTextureDirectory(const std::string& path); 
				common::Handle build(bool loadMaterials = true);
				common::GameObject& buildGet(bool loadMaterials = true); 

			protected:

			private:
				SceneBuilder& sceneBuilder;
				std::unique_ptr<OverrideMaterialProperties> matOverride; 
				bool loadFromDisk = true; 
				const glm::vec4* color = nullptr; 
				glm::vec3 scale = glm::vec3{ 1.0f, 1.0f, 1.0f };
				glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f };
				common::Handle vertShader = common::Handle::getDefault(); 
				common::Handle fragShader = common::Handle::getDefault();
				common::Handle texture = common::Handle::getDefault();
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
				Builder& setType(const common::Type::Light& type); 
				Builder& setLinkedObject(const common::Handle& linkedObject);
				Builder& setPosition(const glm::vec3& position); 
				Builder& setAmbient(const glm::vec4& ambient);
				Builder& setDiffuse(const glm::vec4& diffuse); 
				Builder& setSpecular(const glm::vec4& position); 
				//Set the direction wheree the light is pointing towards
				Builder& setDirection(const glm::vec4& direction);
				Builder& setDiameter(const float& innerDiameter, const float& outerDiameter);
				common::Handle build(); 

			private:
				SceneBuilder& sceneBuilder; 
				const common::Handle* linkedHandle	= nullptr; 
				const common::Type::Light* type		= nullptr; 
				const glm::vec4* lightDirection		= nullptr; 
				const glm::vec3* position			= nullptr; 
				const glm::vec4* ambient			= nullptr; 
				const glm::vec4* diffuse			= nullptr; 
				const glm::vec4* specular			= nullptr; 
				const float* innerDiameter			= nullptr;
				const float* outerDiameter			= nullptr; 
			};
		private:


		};
		//TODO: change this to 'Materials'
		class Materials {
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
				Builder& setBaseColorTexture(const std::string& path);
				Builder& setTexture(common::Handle texture); 
				Builder& setBumpMap(const common::Handle& bumpHandle);
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
				common::Handle texture = common::Handle::getDefault(); 
				common::Handle bumpMap = common::Handle::getDefault(); 
			};
		};

		std::vector<common::Light> lightList;

		SceneBuilder(core::ObjectManager& objectManager, core::MaterialManager& materialManager, core::TextureManager& textureManager, 
			core::MapManager& mapManager, core::LightManager& lightManager) 
			: objectManager(objectManager), materialManager(materialManager), 
			defaultMaterial(&materialManager.resource(common::Handle::getDefault())), textureManager(textureManager), 
			mapManager(mapManager), lightManager(lightManager) { }
		~SceneBuilder() = default;

		//todo: currently this only returns game objects, see if there is way to expand this
		common::GameObject& entity(const common::Handle& handle);
		common::Light& light(const common::Handle& handle);
		common::Material& getMaterial(const common::Handle& handle);

	private:
		core::ObjectManager& objectManager; 
		core::MaterialManager& materialManager; 
		core::TextureManager& textureManager; 
		core::LightManager& lightManager;
		core::MapManager& mapManager; 

		//defaults -- TODO: remove this in favor of each manager having its own default 
		common::Material* defaultMaterial = nullptr; 

		common::Handle addObject(const std::string& pathToFile, glm::vec3& position, glm::vec3& scaleAmt,
			common::Handle* materialHandle, common::Handle& vertShader,
			common::Handle& fragShader, bool loadMaterials,
			std::string* materialFilePath, std::string* textureDir, 
			const glm::vec4* overrideColor = nullptr, const GameObjects::Builder::OverrideMaterialProperties* matPropOverride = nullptr);

		common::Handle addMaterial(const glm::vec4& surfaceColor, const glm::vec4& hightlightColor, const glm::vec4& ambient, 
			const glm::vec4& diffuse, const glm::vec4& specular,
			const int& shinyCoefficient, common::Handle* texture, common::Handle* bumpMap);
		/// <summary>
		/// Create a light object with a linked game object
		/// </summary>
		/// <param name="type"></param>
		/// <param name="position"></param>
		/// <param name="linkedObject"></param>
		/// <param name="ambient"></param>
		/// <param name="diffuse"></param>
		/// <param name="specular"></param>
		/// <param name="direction"></param>
		/// <param name="innerCutoff"></param>
		/// <param name="outerCutoff"></param>
		/// <returns></returns>
		common::Handle addLight(const common::Type::Light& type, const glm::vec3& position, const common::Handle& linkedHandle, 
			const glm::vec4& ambient, const glm::vec4& diffuse, 
			const glm::vec4& specular, const glm::vec4* direction = nullptr, 
			const float* innerCutoff = nullptr, const float* outerCutoff = nullptr); 
		/// <summary>
		/// Create a light object with no linked game object
		/// </summary>
		/// <param name="type"></param>
		/// <param name="position"></param>
		/// <param name="ambient"></param>
		/// <param name="diffuse"></param>
		/// <param name="specular"></param>
		/// <param name="direction"></param>
		/// <param name="innerCutoff"></param>
		/// <param name="outerCutoff"></param>
		/// <returns></returns>
		common::Handle addLight(const common::Type::Light& type, const glm::vec3& position, const glm::vec4& ambient,
			const glm::vec4& diffuse, const glm::vec4& specular, 
			const glm::vec4* direction = nullptr, const float* innerCutoff = nullptr,
			const float* outerCutoff = nullptr);

		friend class common::Mesh::Builder; 
		friend class GameObjects::Builder;
	};
}
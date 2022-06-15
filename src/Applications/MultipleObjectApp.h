#pragma once 

#include "SC/Application.hpp"
#include "SC/ConfigFile.hpp"
#include "SC/Time.hpp"
#include "SC/Camera.hpp"
#include "SC/Interactivity.hpp"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"

namespace star {
	class MultipleObjectApp :
		public common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager>,
		public common::Interactivity
	{
	public:

		MultipleObjectApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager, common::Camera* inCamera);

		void Load();

		void Update();

		void keyCallback(int key, int scancode, int action, int mods) override;

		void mouseMovementCallback(double xpos, double ypos) override;

		void mouseButtonCallback(int button, int action, int mods) override;

		void scrollCallback(double xoffset, double yoffset) override;

	protected:

	private: 
		common::Object* cone = nullptr; 
		common::Object* cube = nullptr; 
	};
}
//#pragma once 
//
//#include "SC/Application.hpp"
//#include "SC/ConfigFile.hpp"
//#include "SC/Time.hpp"
//#include "SC/Camera.hpp"
//#include "SC/GameObject.hpp"
//#include "SC/Interactivity.hpp"
//#include "ShaderManager.h"
//#include "ObjectManager.hpp"
//#include "TextureManager.h"
//#include "LightManager.hpp"
//
//namespace star {
//	class MultipleObjectApp :
//		public common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager, core::LightManager>,
//		public common::Interactivity
//	{
//	public:
//
//		MultipleObjectApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager, core::LightManager* lightManager, common::Camera* inCamera);
//
//		void Load();
//
//		void Update();
//
//		void keyCallback(int key, int scancode, int action, int mods) override;
//
//		void mouseMovementCallback(double xpos, double ypos) override;
//
//		void mouseButtonCallback(int button, int action, int mods) override;
//
//		void scrollCallback(double xoffset, double yoffset) override;
//
//	protected:
//
//	private: 
//		const float moveOscillationAmt = 1.0f; 
//		const float moveSpeed = 10.0f; 
//
//		common::GameObject* cone = nullptr; 
//		common::GameObject* cube = nullptr;
//		common::GameObject* lion = nullptr; 
//
//		bool cubeMovingUp = true; 
//		float cubeCeil = 0.f; 
//		float cubeFloor = 0.f;
//		float cubeMoveAmt = 0.f; 
//		bool coneMovingUp = false; 
//		float coneCeil = 0.f; 
//		float coneFloor = 0.f; 
//	};
//}
//#pragma once 
//
//#include "SC/Application.hpp"
//#include "SC/ConfigFile.hpp"
//#include "SC/Time.hpp"
//#include "SC/Camera.hpp"
//#include "SC/Interactivity.hpp"
//#include "ShaderManager.h"
//#include "ObjectManager.hpp"
//#include "TextureManager.h"
//#include "LightManager.hpp"
//
//#include <GLFW/glfw3.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtx/transform.hpp>
//
//#include <string> 
//#include <memory> 
//
//namespace star{
//    class ObjectApp : 
//        public common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager, core::LightManager>, 
//        public common::Interactivity
//    {
//    public: 
//        ObjectApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager, core::LightManager* lightManager, common::Camera* inCamera); 
//
//        void Load(); 
//
//        void Update(); 
//
//        void keyCallback(int key, int scancode, int action, int mods) override;
//
//        void mouseMovementCallback(double xpos, double ypos) override;
//
//        void mouseButtonCallback(int button, int action, int mods) override;
//
//        void scrollCallback(double xoffset, double yoffset) override; 
//
//    protected: 
//
//    private: 
//        double scaleAmt = 0.1; 
//        static common::GameObject* currentObject; 
//        static bool moveUp, moveDown, moveRight, moveLeft, click; 
//        static glm::vec2 prevMousePosition, prevScroll, mouseMovement; 
//        static double zoomDir;
//        static float ammount; 
//        bool moved = false; 
//       
//    }; 
//}
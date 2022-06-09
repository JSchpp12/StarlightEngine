#pragma once 

#include "SC/Application.hpp"
#include "SC/ConfigFile.hpp"
#include "SC/Time.hpp"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <string> 
#include <memory> 

namespace star{
    class ObjectApp : public common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager>{
    public: 
        ObjectApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager); 

        void Load(); 

        void Update(); 

        static void GLFWKeyHandle(GLFWwindow* window, int key, int scancode, int action, int mods);

        static void GLFWMouseMovement(GLFWwindow* window, double xpos, double ypos); 

        static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods); 

        static void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset); 

    protected: 

    private: 
        double scaleAmt = 0.1; 
        static common::Object* currentObject; 
        static bool moveUp, moveDown, moveRight, moveLeft, click; 
        static glm::vec2 prevMousePosition, prevScroll, mouseMovement; 
        static double zoomDir;
        static float ammount; 
        bool moved = false; 
       
    }; 
}
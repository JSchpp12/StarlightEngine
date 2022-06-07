#pragma once 

#include "SC/Application.hpp"
#include "SC/ConfigFile.hpp"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"

#include <GLFW/glfw3.h>

#include <string> 
#include <memory> 

namespace star{
    class ObjectApp : public common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager>{
    public: 
        ObjectApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager); 

        void Load(); 

        static void GLFWKeyHandle(GLFWwindow* window, int key, int scancode, int action, int mods); 

        void Update(); 

    protected: 

    private: 
        
    }; 
}
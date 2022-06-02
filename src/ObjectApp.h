#pragma once 

#include "SC/Application.hpp"
#include "SC/ConfigFile.hpp"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"

#include <string> 
#include <memory> 

namespace star{
    class ObjectApp : public common::Application{
    public: 
        ObjectApp(common::ConfigFile* configFile, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager); 

        virtual void Load(); 

        virtual void Update(); 

    protected: 

    private: 
        
    }; 
}
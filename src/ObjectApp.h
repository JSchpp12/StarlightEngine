#pragma once 

#include "SC/Application.hpp"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"

#include <string> 
#include <memory> 

namespace star{
    class ObjectApp : public common::Application{
    public: 
        ObjectApp(const std::string& pathConfigFile); 

        virtual void Load(); 

        virtual void Update(); 

    protected: 

    private: 
        
    }; 
}
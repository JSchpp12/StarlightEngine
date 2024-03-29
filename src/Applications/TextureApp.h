#pragma once 

#include "SC/Application.hpp"
#include "SC/ConfigFile.hpp"
#include "SC/Time.hpp"
#include "SC/Camera.hpp"
#include "SC/Interactivity.hpp"
#include "ShaderManager.h"
#include "TextureManager.hpp"
#include "LightManager.hpp"
#include "SceneBuilder.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <string> 
#include <memory> 

namespace star {
    class TextureApp :
        public common::Application<core::ShaderManager, core::TextureManager, core::LightManager, SceneBuilder>,
        public common::Interactivity
    {
    public:
        TextureApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList,
            std::vector<common::Handle>* lightList, core::ShaderManager* shaderManager,
            core::TextureManager* textureManager, core::LightManager* lightManager,
            SceneBuilder& sceneBuilder, common::Camera* inCamera);

        void Load();

        void Update();

        void keyCallback(int key, int scancode, int action, int mods) override;

        void mouseMovementCallback(double xpos, double ypos) override;

        void mouseButtonCallback(int button, int action, int mods) override;

        void scrollCallback(double xoffset, double yoffset) override;

    protected:

    private:
        double scaleAmt = 0.1;
        common::GameObject* lion = nullptr;
        common::Light* pointLight = nullptr;
        std::vector<common::Handle>* lightList = nullptr;

        const float speed = 10.0f;
        float max = 2.0f;
        float min = -2.0f;
        bool movingRight = true;

    };
}
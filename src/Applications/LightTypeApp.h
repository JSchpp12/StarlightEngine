#pragma once 

#include "SC/Application.hpp"
#include "SC/ConfigFile.hpp"
#include "SC/Time.hpp"
#include "SC/Camera.hpp"
#include "SC/Interactivity.hpp"
#include "SC/DebugHelpers.hpp"
#include "ShaderManager.h"
#include "TextureManager.hpp"
#include "LightManager.hpp"
#include "SceneBuilder.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string> 
#include <memory> 

namespace star {
    class LightTypeApp :
        public common::Application<core::ShaderManager, core::TextureManager, core::LightManager, SceneBuilder>,
        public common::Interactivity
    {
    public:
        LightTypeApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList,
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
        const int sunSpeed = 50;
        const int spotSpeed = 1;
        double scaleAmt = 0.1;
        common::GameObject* rock = nullptr;
        common::Light* sun = nullptr;
        common::Light* spot = nullptr; 
        std::vector<common::Handle>* lightList = nullptr;


        static int disabledLightCounter;
        static bool upCounter; 
        static bool rotatingCounterClock; 
        static std::chrono::steady_clock::time_point timeSinceLastUpdate; 

        static bool pressRight; 
        static bool pressLeft; 
        static bool pressUp; 
        static bool pressDown; 
    };
}
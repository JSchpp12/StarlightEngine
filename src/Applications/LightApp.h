#pragma once 

#include "SC/Application.hpp"
#include "SC/ConfigFile.hpp"
#include "SC/Time.hpp"
#include "SC/Camera.hpp"
#include "SC/Interactivity.hpp"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "TextureManager.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <string> 
#include <memory> 

namespace star {
    class LightApp :
        public common::Application<core::ShaderManager, core::ObjectManager, core::TextureManager>,
        public common::Interactivity
    {
    public:
        LightApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, core::ShaderManager* shaderManager, core::ObjectManager* objectManager, core::TextureManager* textureManager, common::Camera* inCamera);

        void Load();

        void Update();

        void keyCallback(int key, int scancode, int action, int mods) override;

        void mouseMovementCallback(double xpos, double ypos) override;

        void mouseButtonCallback(int button, int action, int mods) override;

        void scrollCallback(double xoffset, double yoffset) override;

    protected:

    private:
        double scaleAmt = 0.1;
        static common::GameObject* currentObject;
        static bool moveUp, moveDown, moveRight, moveLeft, click;
        static glm::vec2 prevMousePosition, prevScroll, mouseMovement;
        static double zoomDir;
        static float ammount;
        bool moved = false;

    };
}
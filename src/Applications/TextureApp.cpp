#include "TextureApp.h"

star::TextureApp::TextureApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, std::vector<common::Handle>* lightList,
    core::ShaderManager* shaderManager, core::TextureManager* textureManager,
    core::LightManager* lightManager, SceneBuilder& sceneBuilder,
    common::Camera* inCamera) :
    lightList(lightList), star::common::Application<core::ShaderManager,
    core::TextureManager, core::LightManager, SceneBuilder>(configFile, objectList, shaderManager, textureManager, lightManager, sceneBuilder, inCamera) { }

void star::TextureApp::Load() {
    //load lion 
    auto mediaDirectoryPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory);
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/rapid.obj";
        //auto materialsPath = mediaDirectoryPath + "models/lion-statue/source";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{1.0f, -0.95f, 0.5f})
            .setScale(glm::vec3{ 0.04f, 0.04f, 0.04f })
            .overrideAmbient(glm::vec3{0.5f, 0.5f, 0.5f})
            .build(true)
        );
    }
    this->lion = &this->sceneBuilder.getObject(this->objectList->at(0));
    this->lion->rotateRelative(-90, glm::vec3{ 1.0f, 0.0f, 0.0f });

    //load plant 
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/aloevera/aloevera.obj";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{ -1.0f, 0.0f, -0.0f })
            .setScale(glm::vec3{1.5f, 1.5f, 1.5f})
            .build());
    }
    //table
    {
        auto objectPath = mediaDirectoryPath + "models/table/Desk OBJ.obj";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{ 0.0f, -0.4f, 0.0f })
            .setScale(glm::vec3{ 0.01f, 0.01f, 0.01f })
            .setMaterialFilePath(mediaDirectoryPath + "models/table/")
            .setTextureDirectory(mediaDirectoryPath + "models/table/textures/")
            .build());
    }
    //rock
    {
        auto objectPath = mediaDirectoryPath + "models/rock/898927_rock.obj";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setPosition(glm::vec3{ 0.0f, 0.0f, 0.0f })
            .setScale(glm::vec3{0.05f, 0.05f, 0.05f})
            .overrideDiffuse(glm::vec3{10.0f, 10.0f, 10.0f})
            .overrideSpecular(glm::vec3{10.0f, 10.0f, 10.0f})
            .overrideShiny(512)
            .setMaterial(SceneBuilder::Materials::Builder(this->sceneBuilder)
                .setTexture(this->textureManager->add(common::FileHelpers::GetBaseFileDirectory(objectPath) + "textures/rock_low_Base_Color.png"))
                .build())
            .build());        
    }

    {
        auto objectPath = mediaDirectoryPath + "models/icoSphere/low_poly_icoSphere.obj";
        auto vertShaderPath = mediaDirectoryPath + "models/icoSphere/icoSphere.vert";
        auto fragShaderPath = mediaDirectoryPath + "models/icoSphere/icoSphere.frag";

        //load light
        this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
            .setType(common::Type::Light::point)
            .setPosition(glm::vec3{ -2.0f, 2.0f, 0.0f })
            .setAmbient(glm::vec4{ 1.0f, 1.0f, 0.1f, 0.3f })
            .setDiffuse(glm::vec4{ 1.0f, 1.0f, 0.1f, 0.0f })
            .setSpecular(glm::vec4{ 1.0f, 1.0f, 0.1f, 5.0f })
            .setLinkedObject(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
                .setPath(objectPath)
                .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
                .setColor(glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f })
                .setVertShader(this->shaderManager->add(vertShaderPath))
                .setFragShader(this->shaderManager->add(fragShaderPath))
                .build(false))
            .build());

        this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
            .setType(common::Type::Light::point)
            .setPosition(glm::vec3{0.4f, 0.4f, 0.0f})
            .setAmbient(glm::vec4{ 1.0f, 0.0f, 0.0f, 0.03f })
            .setDiffuse(glm::vec4{ 1.0f, 0.0f, 0.0f, 20.0f })
            .setSpecular(glm::vec4{ 1.0f, 0.0f, 0.0f, 2.0f })
            .setLinkedObject(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
                .setPath(objectPath)
                .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
                .setColor(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f })
                .setVertShader(this->shaderManager->add(vertShaderPath))
                .setFragShader(this->shaderManager->add(fragShaderPath))
                .build(false))
            .build());
        this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
            .setType(common::Type::Light::point)
            .setPosition(glm::vec3{ -1.0f, 0.4f, 0.5f })
            .setAmbient(glm::vec4{ 0.0f, 0.0f, 1.0f, 0.15f })
            .setDiffuse(glm::vec4{ 0.0f, 0.0f, 1.0f, 0.5f })
            .setSpecular(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f })
            .setLinkedObject(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
                .setPath(objectPath)
                .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
                .setColor(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f })
                .setVertShader(this->shaderManager->add(vertShaderPath))
                .setFragShader(this->shaderManager->add(fragShaderPath))
                .build(false))
            .build());
        //this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
        //    .setType(common::Type::Light::point)
        //    .setPosition(glm::vec3{ 1.0f, 0.4f, -0.2f })
        //    .setAmbient(glm::vec4{ 0.5f, 1.0f, 1.0f, 0.5f })
        //    .setDiffuse(glm::vec4{ 0.5f, 1.0f, 1.0f, 5.0f })
        //    .setSpecular(glm::vec4{ 1.0f, 1.0f, 1.0f, 0.5f })
        //    .setPosition(glm::vec3{ 1.0f, 1.0f, 0.0f })
        //    .setLinkedObject(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
        //        .setPath(objectPath)
        //        .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
        //        .setColor(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f })
        //        .setVertShader(this->shaderManager->add(vertShaderPath))
        //        .setFragShader(this->shaderManager->add(fragShaderPath))
        //        .build(false))
        //    .build());
        //this->lightList->push_back(SceneBuilder::Lights::Builder(this->sceneBuilder)
        //    .setType(common::Type::Light::directional)
        //    .setPosition(glm::vec3{ 0.0f, 0.4f, 0.0f })
        //    .setAmbient(glm::vec4{ 1.0f, 1.0f, 1.0f, 0.1f })
        //    .setDiffuse(glm::vec4{ 1.0f, 1.0f, 1.0f, 5.0f })
        //    .setSpecular(glm::vec4{ 1.0f, 1.0f, 1.0f, 5.0f })
        //    .build());
    }
}

void star::TextureApp::Update() {
}

void star::TextureApp::keyCallback(int key, int scancode, int action, int mods) {
}

void star::TextureApp::mouseMovementCallback(double xpos, double ypos) {
}

void star::TextureApp::mouseButtonCallback(int button, int action, int mods) {

}

void star::TextureApp::scrollCallback(double xoffset, double yoffset) {
}
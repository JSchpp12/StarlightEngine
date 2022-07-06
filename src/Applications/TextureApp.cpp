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
    //{
    //    auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/rapid.obj";
    //    auto texturePath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/material0_basecolor.png";
    //    auto textureHandle = this->textureManager->add(texturePath);

    //    this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
    //        .setPath(objectPath)
    //        .setTexture(textureHandle)
    //        .setPosition(glm::vec3{ 0.0f, -0.44f, 0.0f })
    //        //.setMaterial(common::Material::Builder()
    //        //    .setHighlightColor(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f })
    //        //    .setShinyCoefficient(256.0f)
    //        //    .setSurfaceColor(glm::vec4{ 0.7f, 0.1f, 0.1f, 1.0f })
    //        //    .build())
    //        .build()
    //    );
    //}
    //this->lion = this->sceneBuilder.getObject(this->objectList->at(0));
    //this->lion->rotateRelative(-90, glm::vec3{ 1.0f, 0.0f, 0.0f });

    //load plant 
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/aloevera/aloevera.obj";
        //auto texturePath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/cactus/cactus_ceramic_blue_DIF_1k.jpg";
        //auto textureHandle = this->textureManager->add(texturePath);
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            //.setTexture(textureHandle)
            .setPosition(glm::vec3{ 0.0f, -0.44f, 0.0f })
            .loadMaterials()
            //.setMaterial(SceneBuilder::Material::Builder(this->sceneBuilder)
            //    .setHighlightColor(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f })
            //    .setShinyCoefficient(256.0f)
            //    .setSurfaceColor(glm::vec4{ 0.7f, 0.1f, 0.1f, 1.0f })
            //    .buildGet())
            .build());
    }

    {
        //load light
        this->lightList->push_back(this->lightManager->Add(common::Type::Light::point, glm::vec3{ 1.0f, 2.0f, 0.0f }, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }));
        this->pointLight = this->lightManager->Get(this->lightList->at(0));
        this->lightList->push_back(this->lightManager->Add(common::Type::Light::directional, glm::vec3{}, glm::vec4{ 1.0f, 1.0f, 1.0f, 0.005f }));

        auto objectPath = mediaDirectoryPath + "models/icoSphere/low_poly_icoSphere.obj";
        auto vertShaderPath = mediaDirectoryPath + "models/icoSphere/icoSphere.vert";
        auto fragShaderPath = mediaDirectoryPath + "models/icoSphere/icoSphere.frag";

        this->pointLight->setLinkedObjectHandle(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
            .setPosition(this->pointLight->getPosition())
            .setVertShader(this->shaderManager->add(vertShaderPath))
            .setFragShader(this->shaderManager->add(fragShaderPath))
            .build());
        this->pointLight->setScale(glm::vec3{ 0.07f, 0.07f, 0.07f });
        this->pointLight->setLinkedObject(this->sceneBuilder.getObject(this->pointLight->getLinkedObjectHandle()));
    }
}

void star::TextureApp::Update() {
    //this->currentObject->moveRelative(glm::vec3{ 0.0f, 0.0f, 0.001f });\\
    
    auto timePassed = common::Time::timeElapsedLastFrameSeconds();

    //auto position = this->pointLight->getPosition(); 

    //if (!this->movingRight && position.x < this->min) {
    //    this->movingRight = true; 
    //}
    //else if (this->movingRight && position.x > this->max) {
    //    this->movingRight = false; 
    //}

    //this->pointLight->moveRelative(glm::vec3{
    //    this->movingRight ? speed * timePassed : -(speed * timePassed) ,
    //    0.0f,
    //    0.0f });
}

void star::TextureApp::keyCallback(int key, int scancode, int action, int mods) {
}

void star::TextureApp::mouseMovementCallback(double xpos, double ypos)
{
    ////if clicking and moving the mouse, rotate the object
    //if (click) {
    //    auto time = star::common::Time::timeElapsedLastFrameSeconds(); 

    //    glm::vec2 currMousePosition = glm::vec2{ xpos, ypos }; 
    //    glm::vec2 mouseMovementDirection = glm::vec2{
    //        currMousePosition.x - prevMousePosition.x,
    //        currMousePosition.y - prevMousePosition.y
    //    };

    //    ammount = glm::distance(currMousePosition, prevMousePosition) * 0.1;
    //    //std::cout << currMousePosition.x << "," << currMousePosition.y << std::endl;
    //    //std::cout << prevMousePosition.x << "," << prevMousePosition.y << std::endl;
    //    //std::cout << ammount << std::endl;

    //    mouseMovement = mouseMovementDirection * ammount; 
    //    prevMousePosition = currMousePosition; 
    //}
}

void star::TextureApp::mouseButtonCallback(int button, int action, int mods)
{
}

void star::TextureApp::scrollCallback(double xoffset, double yoffset)
{

}
#include "LightApp.h"

star::LightApp::LightApp(common::ConfigFile* configFile, std::vector<common::Handle>* objectList, std::vector<common::Handle>* lightList,
    core::ShaderManager* shaderManager, core::TextureManager* textureManager, 
    core::LightManager* lightManager, SceneBuilder& sceneBuilder,
    common::Camera* inCamera) :
    lightList(lightList), star::common::Application<core::ShaderManager,
    core::TextureManager, core::LightManager, SceneBuilder>(configFile, objectList, shaderManager, textureManager, lightManager, sceneBuilder, inCamera) { }

void star::LightApp::Load() {
    //load lion 
    auto mediaDirectoryPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory); 
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/rapid.obj";
        auto texturePath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/lion-statue/source/material0_basecolor.png";
        auto textureHandle = this->textureManager->add(texturePath);
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setTexture(textureHandle)
            .setPosition(glm::vec3{ 0.0f, -0.44f, 0.0f })
            .setMaterial(SceneBuilder::Material::Builder(this->sceneBuilder)
                .setHighlightColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f})
                .setShinyCoefficient(256.0f)
                .setSurfaceColor(glm::vec4{0.7f, 0.1f, 0.1f, 1.0f})
                .buildGet())
            .build()
        );
    }
    this->lion = this->sceneBuilder.getObject(this->objectList->at(0));
    //this->lion->moveRelative(glm::vec3{ 0.0f, -0.7f, 0.0f });
    this->lion->rotateRelative(-90, glm::vec3{ 1.0f, 0.0f, 0.0f });

    {
        auto objectPath = mediaDirectoryPath + "models/cone/cone.obj";
        auto texturePath = mediaDirectoryPath + "models/cone/ConeTexture.png";
        auto vertShaderPath = mediaDirectoryPath + "shaders/defaultVert.vert";
        auto fragShaderPath = mediaDirectoryPath + "shaders/defaultFrag.frag";

        auto vertShader = this->shaderManager->add(vertShaderPath);
        auto fragShader = this->shaderManager->add(fragShaderPath);
        auto textureHandle = this->textureManager->add(texturePath);
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setTexture(textureHandle)
            .setScale(glm::vec3{ 0.2f, 0.2f, 0.2f })
            .setPosition(glm::vec3{ 1.2f, 0.21f, 0.5f })
            .setMaterial(SceneBuilder::Material::Builder(this->sceneBuilder)
                .setHighlightColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f })
                .setSurfaceColor(glm::vec4{ 0.5f, 0.5f, 0.5f, 1.0f })
                .setShinyCoefficient(1)
                .buildGet())
            .build());
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setTexture(textureHandle)
            .setScale(glm::vec3{ 0.2f, 0.2f, 0.2f })
            .setPosition(glm::vec3{ -1.3f, 0.21f, -0.5f })
            .setMaterial(SceneBuilder::Material::Builder(this->sceneBuilder)
                .setHighlightColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f })
                .setSurfaceColor(glm::vec4{ 0.5f, 0.5f, 0.5f, 1.0f })
                .setShinyCoefficient(1)
                .buildGet())
            .build());
    }
    this->cone = this->sceneBuilder.getObject(this->objectList->at(1));
    this->conetwo = this->sceneBuilder.getObject(this->objectList->at(2));

    //load quad 
    {
        auto objectPath = this->configFile->GetSetting(star::common::Config_Settings::mediadirectory) + "models/quad/quad.obj";
        this->objectList->push_back(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setMaterial(SceneBuilder::Material::Builder(this->sceneBuilder)
                .setHighlightColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f})
                .setSurfaceColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f })
                .setShinyCoefficient(256)
                .buildGet())
            .setScale(glm::vec3{ 2.0f, 2.0f, 2.0f })
            .setPosition(glm::vec3{ 0.0f, 0.0f, 0.0f })
            .build());
    }
    this->floor = this->sceneBuilder.getObject(this->objectList->at(2));

    {
        //load light
        this->lightList->push_back(this->lightManager->Add(common::Type::Light::point, glm::vec3{ -1.0f, 0.4f, 0.0f }, glm::vec4{ 0.0f, 0.25f, 0.5f, 0.12f }));
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

        this->lightList->push_back(this->lightManager->Add(common::Type::Light::point, glm::vec3{ 1.0f, 0.4f, 0.0f }, glm::vec4{ 0.0f, 0.0f, 1.0f, 0.12f }));
        this->pointLightTwo = this->lightManager->Get(this->lightList->at(2));
        this->pointLightTwo->setLinkedObjectHandle(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
            .setPosition(this->pointLightTwo->getPosition())
            .setVertShader(this->shaderManager->add(vertShaderPath))
            .setFragShader(this->shaderManager->add(fragShaderPath))
            .build());
        this->pointLightTwo->setScale(glm::vec3{ 0.07f, 0.07f, 0.07f });
        this->pointLightTwo->setLinkedObject(this->sceneBuilder.getObject(this->pointLightTwo->getLinkedObjectHandle()));


        this->lightList->push_back(this->lightManager->Add(common::Type::Light::point, glm::vec3{ 0.0f, 0.4f, 1.0f }, glm::vec4{ 0.0f, 1.0f, 0.0f, 0.12f }));
        this->pointLightThree = this->lightManager->Get(this->lightList->at(3));
        this->pointLightThree->setLinkedObjectHandle(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
            .setPosition(this->pointLightThree->getPosition())
            .setVertShader(this->shaderManager->add(vertShaderPath))
            .setFragShader(this->shaderManager->add(fragShaderPath))
            .build());
        this->pointLightThree->setScale(glm::vec3{ 0.07f, 0.07f, 0.07f });
        this->pointLightThree->setLinkedObject(this->sceneBuilder.getObject(this->pointLightThree->getLinkedObjectHandle()));

        this->lightList->push_back(this->lightManager->Add(common::Type::Light::point, glm::vec3{ 0.0f, 0.4f, -1.0f }, glm::vec4{ 0.1f, 0.0f, 0.0f, 0.12f }));
        this->pointLightFour = this->lightManager->Get(this->lightList->at(4));
        this->pointLightFour->setLinkedObjectHandle(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
            .setPosition(this->pointLightFour->getPosition())
            .setVertShader(this->shaderManager->add(vertShaderPath))
            .setFragShader(this->shaderManager->add(fragShaderPath))
            .build());
        this->pointLightFour->setScale(glm::vec3{ 0.07f, 0.07f, 0.07f });
        this->pointLightFour->setLinkedObject(this->sceneBuilder.getObject(this->pointLightFour->getLinkedObjectHandle()));

        this->lightList->push_back(this->lightManager->Add(common::Type::Light::point, glm::vec3{ this->cone->getPosition().x, this->cone->getPosition().y + 0.27, this->cone->getPosition().z }, glm::vec4{ 1.0f, 1.0f, 1.0f, 0.02f }));
        this->pointLightFive = this->lightManager->Get(this->lightList->at(5));
        this->pointLightFive->setLinkedObjectHandle(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
            .setPosition(this->pointLightFive->getPosition())
            .setVertShader(this->shaderManager->add(vertShaderPath))
            .setFragShader(this->shaderManager->add(fragShaderPath))
            .build());        
        this->lightList->push_back(this->lightManager->Add(common::Type::Light::point, glm::vec3{ this->conetwo->getPosition().x, this->conetwo->getPosition().y + 0.27, this->conetwo->getPosition().z }, glm::vec4{ 1.0f, 1.0f, 1.0f, 0.02f }));
        this->pointLightSix = this->lightManager->Get(this->lightList->at(6));
        this->pointLightSix->setLinkedObjectHandle(SceneBuilder::GameObjects::Builder(this->sceneBuilder)
            .setPath(objectPath)
            .setScale(glm::vec3{ 0.07f, 0.07f, 0.07f })
            .setPosition(this->pointLightSix->getPosition())
            .setVertShader(this->shaderManager->add(vertShaderPath))
            .setFragShader(this->shaderManager->add(fragShaderPath))
            .build());
    }
}

void star::LightApp::Update() {
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

void star::LightApp::keyCallback(int key, int scancode, int action, int mods) {
}

void star::LightApp::mouseMovementCallback(double xpos, double ypos)
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

void star::LightApp::mouseButtonCallback(int button, int action, int mods)
{
}

void star::LightApp::scrollCallback(double xoffset, double yoffset)
{

}
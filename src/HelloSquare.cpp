/*
* Date: 12/12/2021
* Based on code from vulkan-tutorial.com -- "Drawing a triangle"
*/
#include "HelloSquareApplication.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "ShadercHelper.h"

int main() {
    std::unique_ptr<HelloSquareApplication> app; 

    //storage for graphics objects
    std::unique_ptr<std::vector<VulkanObject>> objectList(new std::vector<VulkanObject>);

    //populate object list 
    {
        //both vertex and vert attribute data is contained in one array of verticies == 'interleaving vertex' attributes
        //conatiner for storing new objects when loading
        //std::unique_ptr<std::vector<Vertex>> newVerticies(new std::vector<Vertex>{
        //    {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        //    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        //    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        //    {{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        //    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        //    {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        //    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        //    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        //});

        //std::unique_ptr<std::vector<uint16_t>> newIndicies(new std::vector<uint16_t>{
        //    //0, 1, 2, 2, 3, 0
        //    0, 1, 2, 2, 3, 0,
        //    0, 3, 7, 7, 4, 0,
        //    1, 0, 4, 4, 5, 1,
        //    2, 1, 5, 5, 6, 2,
        //    3, 2, 6, 6, 7, 3,
        //    4, 5, 6, 6, 7, 4
        //});

        VulkanObject newObject = VulkanObject("media/models/lion-statue/source/rapid.obj");
        newObject.AddShader(vk::ShaderStageFlagBits::eVertex, "media/models/lion-statue/vertShader.vert"); 
        newObject.AddShader(vk::ShaderStageFlagBits::eFragment, "media/models/lion-statue/fragShader.frag"); 

        //load objects into list
        objectList->push_back(newObject);

        app = std::make_unique<HelloSquareApplication>(HelloSquareApplication(objectList.get())); 
    }
   

    try {
        app->run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        do {
            std::cout << "Press a key to exit..." << std::endl;
        } while (std::cin.get() != '\n');
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
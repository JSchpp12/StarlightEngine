#include "HelloSquareApplication.h"

typedef std::chrono::high_resolution_clock Clock;

bool HelloSquareApplication::spin = true; 

void HelloSquareApplication::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        NULL; 
    }
}

void HelloSquareApplication::mainLoop() {
    int frameCount = 0;


    auto start = Clock::now();
    lastColorUpdateTime = std::chrono::high_resolution_clock::now(); 
    vertexColors.resize(numVerticies);
    std::cout << "The number of frames drawn in the last second will be displayed below" << std::endl; 

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
        frameCount++;

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
        if (duration >= 1000) {
            std::cout << "Frames: " << frameCount << std::endl;
            frameCount = 0;
            start = Clock::now();
        }
    }

    //wait until the device is complete with all drawing operations before cleanup
    vkDeviceWaitIdle(device);
}

void HelloSquareApplication::drawFrame() {
    /* Goals of each call to drawFrame:
    *   get an image from the swap chain
    *   execute command buffer with that image as attachment in the framebuffer
    *   return the image to swapchain for presentation
    * by default each of these steps would be executed asynchronously so need method of synchronizing calls with rendering
    * two ways of doing this:
    *   1. fences
    *       accessed through calls to vkWaitForFences
    *       designed to synchronize appliecation itself with rendering ops
    *   2. semaphores
    *       designed to synchronize opertaions within or across command queues
    * need to sync queu operations of draw and presentation commmands -> using semaphores
    */

    //wait for fence to be ready 
    // 3. 'VK_TRUE' -> waiting for all fences
    // 4. timeout 
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    VkResult result; //swapchain status

    /* Get Image From Swapchain */
    uint32_t imageIndex;

    //as is extension we must use vk*KHR naming convention
    //UINT64_MAX -> 3rd argument: used to specify timeout in nanoseconds for image to become available
    /* Suboptimal SwapChain notes */
        //vulkan can return two different flags 
        // 1. VK_ERROR_OUT_OF_DATE_KHR: swap chain has become incompatible with the surface and cant be used for rendering. (Window resize)
        // 2. VK_SUBOPTIMAL_KHR: swap chain can still be used to present to the surface, but the surface properties no longer match
    result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //the swapchain is no longer optimal according to vulkan. Must recreate a more efficient swap chain
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        //for VK_SUBOPTIMAL_KHR can also recreate swap chain. However, chose to continue to presentation stage
        throw std::runtime_error("failed to aquire swap chain image");
    }

    //check if a previous frame is using the current image
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    //mark image as now being in use by this frame by assigning the fence to it 
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    updateUniformBuffer(imageIndex); 


    /* Command Buffer */
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };

    //where in the pipeline should the wait happen, want to wait until image becomes available
    //wait at stage of color attachment -> theoretically allows for shader execution before wait 
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; //each entry corresponds through index to waitSemaphores[]
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    //which command buffers to submit for execution -- should submit command buffer that binds the swap chain image that was just acquired as color attachment
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &graphicsCommandBuffers[imageIndex];

    //what semaphores to signal when command buffers have finished
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    //set fence to unsignaled state
    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer");
    }

    /* Presentation */
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    //what to wait for 
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    //what swapchains to present images to 
    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    //can use this to get results from swap chain to check if presentation was successful
    presentInfo.pResults = nullptr; // Optional

    //make call to present image
    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResized) {
        frameBufferResized = false;
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image");
    }

    //advance to next frame
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void HelloSquareApplication::cleanup() {
    cleanupSwapChain();

    vkDestroySampler(device, textureSampler, nullptr); 
    vkDestroyImageView(device, textureImageView, nullptr); 
    vkDestroyImage(device, textureImage, nullptr); 
    vkFreeMemory(device, textureImageMemory, nullptr); 

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

    vkDestroyBuffer(device, indexBuffer, nullptr);
    vkFreeMemory(device, indexBufferMemory, nullptr);
    vkDestroyBuffer(device, vertexBuffer, nullptr);
    vkFreeMemory(device, vertexBufferMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device, transferCommandPool, nullptr);
    vkDestroyCommandPool(device, graphicsCommandPool, nullptr);

    vkDestroyDevice(device, nullptr);

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);

    glfwTerminate();
}

void HelloSquareApplication::cleanupSwapChain() {
    vkDestroyImageView(device, depthImageView, nullptr); 
    vkDestroyImage(device, depthImage, nullptr); 
    vkFreeMemory(device, depthImageMemory, nullptr); 

    //delete framebuffers 
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    vkFreeCommandBuffers(device, graphicsCommandPool, static_cast<uint32_t>(graphicsCommandBuffers.size()), graphicsCommandBuffers.data());

    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);

    //destroy image views 
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr); 
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(device, descriptorPool, nullptr); 
}

HelloSquareApplication::HelloSquareApplication(std::vector<VulkanObject>* objectList):
    objectList(objectList)
{
    //generate any variables needed for debug
    //specifically those that take longer to generate 

    //number of verticies in the objectList 
    for (size_t i = 0; i < this->objectList->size(); i++) {
        this->numVerticies += this->objectList->at(i).GetVerticies().size();
        this->numIndicies += this->objectList->at(i).GetIndicies().size(); 
    }
}

void HelloSquareApplication::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void HelloSquareApplication::initVulkan() {
    createInstance();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout(); 
    createGraphicsPipeline();
    createDepthResources();
    createFramebuffers();
    createCommandPools();
    createTextureImage(); 
    createTextureImageView(); 
    createTextureSampler(); 
    createVertexBuffer();
    createIndexBuffer(); 
    createRenderingBuffers(); 
    createDescriptorPool(); 
    createDescriptorSets(); 
    createCommandBuffers();
    createSemaphores();
    createFences();
    createFenceImageTracking();
    std::cout << "Finished Vulkan Init \n";
}

void HelloSquareApplication::createSurface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface");
    }
}

void HelloSquareApplication::initWindow() {
    //actually make sure to init glfw
    glfwInit();
    //tell GLFW to create a window but to not include a openGL instance as this is a default behavior
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //disable resizing functionality in glfw as this will not be handled in the first tutorial
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    //create a window, 3rd argument allows selection of monitor, 4th argument only applies to openGL
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    //need to give GLFW a pointer to current instance of this class
    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    //set input callback events 
    auto callback = glfwSetKeyCallback(window, HelloSquareApplication::key_callback); 
}

void HelloSquareApplication::createSwapChain() {
    //TODO: current implementation requires halting to all rendering when recreating swapchain. Can place old swap chain in oldSwapChain field 
    //  in order to prevent this and allow rendering to continue
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    //how many images should be in the swap chain 
    //in order to avoid extra waiting for driver overhead, author of tutorial recommends +1 of the minimum
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    //with this additional +1 make sure not to go over maximum permitted 
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    //specify image information for the surface 
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; //1 unless using 3D display 
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //how are these images going to be used? Color attachment since we are rendering to them (can change for postprocessing effects)

    QueueFamilyIndices indicies = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndicies[] = { indicies.graphicsFamily.value(), indicies.transferFamily.value(), indicies.presentFamily.value() };

    if (indicies.graphicsFamily != indicies.presentFamily && indicies.presentFamily != indicies.transferFamily) {
        /*need to handle how images will be transferred between different queues
        * so we need to draw images on the graphics queue and then submitting them to the presentation queue
        * Two ways of handling this:
        * 1. VK_SHARING_MODE_EXCLUSIVE: an image is owned by one queue family at a time and can be transferred between groups
        * 2. VK_SHARING_MODE_CONCURRENT: images can be used across queue families without explicit ownership
        */
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 3;
        createInfo.pQueueFamilyIndices = queueFamilyIndicies;
    }
    else if (indicies.graphicsFamily != indicies.presentFamily && indicies.presentFamily == indicies.transferFamily) {
        uint32_t explicitQueueFamilyInd[] = { indicies.graphicsFamily.value(), indicies.presentFamily.value() };
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndicies;
    }
    else {
        //same family is used for graphics and presenting
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; //optional
        createInfo.pQueueFamilyIndices = nullptr; //optional
    }

    //can specify certain transforms if they are supported (like 90 degree clockwise rotation)
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    //what present mode is going to be used
    createInfo.presentMode = presentMode;
    //if clipped is set to true, we dont care about color of pixes that arent in sight -- best performance to enable this
    createInfo.clipped = VK_TRUE;

    //for now, only assume we are making one swapchain
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain");
    }

    //get images in the newly created swapchain 
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    //save swapChain information for later use
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void HelloSquareApplication::recreateSwapChain() {
    int width = 0, height = 0;
    //check for window minimization and wait for window size to no longer be 0
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }
    //wait for device to finish any current actions
    vkDeviceWaitIdle(device);

    cleanupSwapChain();

    //create swap chain itself 
    createSwapChain();

    //image views depend directly on swap chain images so these need to be recreated
    createImageViews();

    //render pass depends on the format of swap chain images
    createRenderPass();

    //viewport and scissor rectangle size are declared during pipeline creation, so the pipeline must be recreated
    //can use dynamic states for viewport and scissor to avoid this 
    createGraphicsPipeline();

    createDepthResources(); 

    createFramebuffers();

    //uniform buffers are dependent on the number of swap chain images, will need to recreate since they are destroyed in cleanupSwapchain()
    createRenderingBuffers();

    createDescriptorPool(); 

    createDescriptorSets(); 

    createCommandBuffers();
}

bool HelloSquareApplication::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }
    return true;
}

void HelloSquareApplication::createInstance() {
    uint32_t glfwExtensionCount = 0;
    uint32_t extensionCount = 0;
    uint32_t glfwRequiredExtensionEnum = 0;

    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available");
    }

    //get required vulkan extensions from glfw
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<VkExtensionProperties> requiredExtensions(**glfwExtensions);
    //enumerate required extensions
    vkEnumerateInstanceExtensionProperties(nullptr, &glfwExtensionCount, requiredExtensions.data());

    //get a count of the number of supported extensions on the system
    //first argument is a filter for type -- leaving null to get all 
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    //query the extension details
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    int foundExtensions = 0;
    for (const auto& extension : requiredExtensions) {
        bool found = false;
        for (const auto& availableExtension : extensions) {
            if (found) {
                foundExtensions++;
                break;
            }
            found = ((*extension.extensionName == *availableExtension.extensionName) && (extension.specVersion == availableExtension.specVersion));
        }
    }
    if (foundExtensions != glfwExtensionCount) {
        throw std::runtime_error("Not all required extensions found for glfw");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    /*
    All vulkan objects follow this pattern of creation :
    1.pointer to a struct with creation info
        2.pointer to custom allocator callbacks, (nullptr) here
        3.pointer to the variable that stores the handle to the new object
    */
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void HelloSquareApplication::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    //check devices and see if they are suitable for use
    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find suitable GPU!");
    }
}

//Check if the given physical device is suitable for vulkan use
bool HelloSquareApplication::isDeviceSuitable(VkPhysicalDevice device) {
    /*
    Method of querying specific information about a device and checking if that device features support for a geometryShader
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
*/
    bool swapChainAdequate = false;
    QueueFamilyIndices indicies = findQueueFamilies(device);
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures); 

    return indicies.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy; 
}

bool HelloSquareApplication::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    //iterate through extensions looking for those that are required
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

HelloSquareApplication::QueueFamilyIndices HelloSquareApplication::findQueueFamilies(VkPhysicalDevice device) {
    uint32_t queueFamilyCount = 0;
    QueueFamilyIndices indicies;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    //need to find a graphicsQueue that supports VK_QUEUE_GRAPHICS_BIT 
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        //pick the family that supports presenting to the display 
        if (presentSupport) {
            indicies.presentFamily = i;
        }
        //pick family that has graphics support
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indicies.graphicsFamily = i;
        }
        else if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            //for transfer family, pick family that does not support graphics but does support transfer queue
            indicies.transferFamily = i;
        }

        //--COULD DO :: pick a device that supports both of these in the same queue for increased performance--
        i++;
    }

    return indicies;
}

void HelloSquareApplication::createLogicalDevice() {
    float queuePrioriy = 1.0f;
    QueueFamilyIndices indicies = findQueueFamilies(physicalDevice);

    //need multiple structs since we now have a seperate family for presenting and graphics 
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indicies.graphicsFamily.value(), indicies.presentFamily.value(), indicies.transferFamily.value() };

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        //create a struct to contain the information required 
        //create a queue with graphics capabilities
        VkDeviceQueueCreateInfo  queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        //most drivers support only a few queue per queueFamily 
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePrioriy;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    //specifying device features that we want to use -- can pull any of the device features that was queried before...for now use nothing
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE; 

    //Create actual logical device
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;

    //specify specific instance info but it is device specific this time
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    //call to create the logical device 
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device");
    }

    vkGetDeviceQueue(device, indicies.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indicies.presentFamily.value(), 0, &presentQueue);
    vkGetDeviceQueue(device, indicies.transferFamily.value(), 0, &transferQueue);
}

uint32_t HelloSquareApplication::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    //query available memory -- right now only concerned with memory type, not the heap that it comes from
    /*VkPhysicalDeviceMemoryProperties contains:
        1. memoryTypes
        2. memoryHeaps - distinct memory resources (dedicated VRAM or swap space)
    */
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        //use binary AND to test each bit (Left Shift)
        //check memory types array for more detailed information on memory capabilities
            //we need to be able to write to memory, so speficially looking to be able to MAP to the memory to write to it from the CPU -- VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        //also need VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type");
}

HelloSquareApplication::SwapChainSupportDetails HelloSquareApplication::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;
    uint32_t formatCount, presentModeCount;

    //get surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &presentModeCount, nullptr);

    if (formatCount != 0) {
        //resize vector in order to hold all available formats
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    if (presentModeCount != 0) {
        //resize for same reasons as format 
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR HelloSquareApplication::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        //check if a format allows 8 bits for R,G,B, and alpha channel
        //use SRGB color space
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    //if nothing matches what we are looking for, just take what is available
    return availableFormats[0];
}

VkPresentModeKHR HelloSquareApplication::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    /*
    * There are a number of swap modes that are in vulkan
    * 1. VK_PRESENT_MODE_IMMEDIATE_KHR: images submitted by application are sent to the screen right away -- can cause tearing
    * 2. VK_PRESENT_MODE_FIFO_RELAXED_KHR: images are placed in a queue and images are sent to the display in time with display refresh (VSYNC like). If queue is full, application has to wait
    *   "Vertical blank" -> time when the display is refreshed
    * 3. VK_PRESENT_MODE_FIFO_RELAXED_KHR: same as above. Except if the application is late, and the queue is empty: the next image submitted is sent to display right away instead of waiting for next blank.
    * 4. VK_PRESENT_MODE_MAILBOX_KHR: similar to #2 option. Instead of blocking applicaiton when the queue is full, the images in the queue are replaced with newer images.
    *   This mode can be used to render frames as fast as possible while still avoiding tearing. Kind of like "tripple buffering". Does not mean that framerate is unlocked however.
    *   Author of tutorial statement: this mode [4] is a good tradeoff if energy use is not a concern. On mobile devices it might be better to go with [2]
    */

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    //only VK_PRESENT_MODE_FIFO_KHR is guaranteed to be available
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D HelloSquareApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    /*
    * "swap extent" -> resolution of the swap chain images (usually the same as window resultion
    * Range of available resolutions are defined in VkSurfaceCapabilitiesKHR
    * Resultion can be changed by setting value in currentExtent to the maximum value of a uint32_t
    *   then: the resolution can be picked by matching window size in minImageExtent and maxImageExtent
    */
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }
    else {
        //vulkan requires that resultion be defined in pixels -- if a high DPI display is used, screen coordinates do not match with pixels
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        //(clamp) -- keep the width and height bounded by the permitted resolutions 
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void HelloSquareApplication::createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    //need to create an imageView for each of the images available
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        //VkImageViewCreateInfo createInfo{};
        //createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        //createInfo.image = swapChainImages[i];

        ////specify how the image will be interpreted
        //createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        //createInfo.format = swapChainImageFormat;

        ////the next fields allows to swizzle RGB values -- leaving as defaults for now 
        //createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        //createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        //createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        //createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        ////subresourceRange describes image purpose -- this use is color targets without any mipmapping levels or multiple layers
        //createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        //createInfo.subresourceRange.baseMipLevel = 0;
        //createInfo.subresourceRange.levelCount = 1;
        //createInfo.subresourceRange.baseArrayLayer = 0;
        //createInfo.subresourceRange.layerCount = 1;

        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT); 
        //if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
        //    throw std::runtime_error("failed to create image views");
        //}

    }
}

void HelloSquareApplication::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding setLayoutBindings;

    /* Binding 0 : Uniform buffers (MVP matricies) */
    setLayoutBindings.binding = 0;
    setLayoutBindings.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;    //for this, we are using a uniform buffer object (UBO)
    setLayoutBindings.descriptorCount = 1;                                   //can pass an array of uniform buffer objects, for this we are only using one 

    //which shader stages are going to use the descriptor 
    setLayoutBindings.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    //only needed for image sampling related descriptors -- not used now
    setLayoutBindings.pImmutableSamplers = nullptr;

    //binding for combined image sampler descriptor
    VkDescriptorSetLayoutBinding samplerLayoutBinding{}; 
    samplerLayoutBinding.binding = 1; 
    samplerLayoutBinding.descriptorCount = 1; 
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; 
    samplerLayoutBinding.pImmutableSamplers = nullptr; 
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;                     //use in the fragment shader

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { setLayoutBindings, samplerLayoutBinding }; 
    VkDescriptorSetLayoutCreateInfo layoutInfo{}; 
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size()); 
    layoutInfo.pBindings = bindings.data(); 

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!"); 
    }
}

VkShaderModule HelloSquareApplication::createShaderModule(std::vector<uint32_t>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = 4*code.size();
    createInfo.pCode = code.data(); 

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module");
    }

    return shaderModule;
}

VkShaderModule HelloSquareApplication::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module");
    }

    return shaderModule;
}

void HelloSquareApplication::createGraphicsPipeline() {
    auto currObject = &this->objectList->at(0); 

    //auto fragShaderCode = readFile("media/shaders/fragShader.frag.spv");
    //auto vertShaderCode = readFile("media/shaders/vertShader.vert.spv");
    

    auto bindingDescriptions = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    auto tmp = currObject->GetVertShader().GetSpirV(); 

    VkShaderModule vertShaderModule = createShaderModule(currObject->GetVertShader().GetSpirV());
    VkShaderModule fragShaderModule = createShaderModule(currObject->GetFragShader().GetSpirV());

    //assign each shader module to a specific stage of the graphics pipeline
    //vert shader first
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main"; //the function to invoke in the shader module
    //optional member -> pSpecializationInfo: 
    //  allows specification for values to shader constants. Use a single single shader module whos function could be customized through this optional value. 
    //  if not useing, set to nullptr which is done automatically in this case with the constructor of the struct. 
    //  Additionally: it is a good choice to use this value instead of variables so that graphics driver can remove if statements if needed for optimization

    //create pipeline info for fragment shader 
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    //store these creation infos for later use 
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    //pVertexBindingDescriptions and pVertexAttributeDescription -> point to arrays of structs to load vertex data
    //for now: leaving blank as the verticies are hard coded in the shaders
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescriptions;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    /*
    VkPipelineInputAssemblyStateCreateInfo -> Describes 2 things:
        1.what kind of geometry will be drawn
            described in topology member:
                -VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from verticies
                -VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 verticies without reuse
                -VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: line strip
                -VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: triangle from every 3 verticies without reuse
                -VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: 2nd and 3rd vertex of every triangle are used as first two verticies of the next triangle
        2.if primitive restart should be enabled
    */
    //Verticies are normally loaded from vertex buffer in sequential order 
    //element buffer can be used to specify this information manually
    //this allows the reuse of verticies!
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    /* Viewport */
    //Viewport describes the region of the framebuffer where the output will be rendered to
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;

    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    //Specify values range of depth values to use for the framebuffer. If not doing anything special, leave at default
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    /* Scissor */
    //this defines in which regions pixels will actually be stored. 
    //any pixels outside will be discarded 

    //we just want to draw the whole framebuffer for now
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainExtent;

    //put scissor and viewport together into struct for creation 
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    /* Rasterizer */
    //takes the geometry and creates fragments which are then passed onto the fragment shader 
    //also does: depth testing, face culling, and the scissor test
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    //if set to true -> fragments that are beyond near and far planes are set to those distances rather than being removed
    rasterizer.depthClampEnable = VK_FALSE;

    //polygonMode determines how frags are generated. Different options: 
    //1. VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
    //2. VK_POLYGON_MODE_LINE: polygon edges are drawn as lines 
    //3. VK_POLYGON_MODE_POINT: polygon verticies are drawn as points
    //NOTE: using any other than fill, requires GPU feature
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;

    //available line widths, depend on GPU. If above 1.0f, required wideLines GPU feature
    rasterizer.lineWidth = 1.0f; //measured in fragment widths

    //cullMode : type of face culling to use.
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    //depth values can be used in way that is known as 'shadow mapping'. 
    //rasterizer is capable of changing depth values through constant addition or biasing based on frags slope 
    //this is left as off for now 
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; //optional 
    rasterizer.depthBiasClamp = 0.0f; //optional 
    rasterizer.depthBiasSlopeFactor = 0.0f; //optional

    /* Multisampling */
    //this is one of the methods of performing anti-aliasing
    //enabling requires GPU feature -- left off for this tutorial 
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; //optional 
    multisampling.pSampleMask = nullptr; //optional
    multisampling.alphaToCoverageEnable = VK_FALSE; //optional
    multisampling.alphaToOneEnable = VK_FALSE; //optional

    /* Depth and Stencil Testing */
    //if using depth or stencil buffer, a depth and stencil tests are neeeded
    VkPipelineDepthStencilStateCreateInfo depthStencil{}; 
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO; 
    depthStencil.depthTestEnable = VK_TRUE;             //specifies if depth of new fragments should be compared to the depth buffer to test for actual display state
    depthStencil.depthWriteEnable = VK_TRUE;            //specifies if the new depth of fragments that pass the depth tests should be written to the depth buffer 
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;   //comparison that is performed to keep or discard fragments - here this is: lower depth = closer, so depth of new frags should be less
    //following are for optional depth bound testing - keep frags within a specific range 
    depthStencil.depthBoundsTestEnable = VK_FALSE;      
    depthStencil.minDepthBounds = 0.0f;                 //optional 
    depthStencil.maxDepthBounds = 1.0f;                 //optional
    //following are used for stencil tests - make sure that format of depth image contains a stencil component
    depthStencil.stencilTestEnable = VK_FALSE; 
    depthStencil.front = {}; 
    depthStencil.back = {}; 

    /* Color blending */
    // after the fragShader has returned a color, it must be combined with the color already in the framebuffer
    // there are two ways to do this: 
    //      1. mix the old and new value to produce final colo r
    //      2. combine the old a new value using a bitwise operation 
    //two structs are needed to create this functionality: 
    //  1. VkPipelineColorBlendAttachmentState: configuration per attached framebuffer 
    //  2. VkPipelineColorBlendStateCreateInfo: global configuration
    //only using one framebuffer in this project -- both of these are disabled in this project
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    /* Dynamic State */
    //some parts of the pipeline can be changed without recreating the entire pipeline
    //if this is defined, the data for the dynamic structures will have to be provided at draw time
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount = 2;
    dynamicStateInfo.pDynamicStates = dynamicStates;

    /* Pipeline Layout */
    //uniform values in shaders need to be defined here 
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout");
    }

    /* Pipeline */
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    //ref all previously created structs
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil; 
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = pipelineLayout;
    //render pass info - ensure renderpass is compatible with pipeline --check khronos docs
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0; //index where the graphics pipeline will be used 
    //allow switching to new pipeline (inheritance) 
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional -- handle to existing pipeline that is being switched to
    pipelineInfo.basePipelineIndex = -1; // Optional

    //finally creating the pipeline -- this call has the capability of creating multiple pipelines in one call
    //2nd arg is set to null -> normally for graphics pipeline cache (can be used to store and reuse data relevant to pipeline creation across multiple calls to vkCreateGraphicsPipeline)
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline");
    }

    //destroy the shader modules that were created 
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
}

void HelloSquareApplication::createRenderPass() {
    /*  Single render pass consists of many small subpasses
        each subpasses are subsequent rendering operations that depend on the contents of framebuffers in the previous pass.
        It is best to group these into one rendering pass, then vulkan can optimize for this in order to save memory bandwidth.
        For this program, we are going to stick with one subpass
    */
    /* Depth attachment */
    VkAttachmentDescription depthAttachment{}; 
    depthAttachment.format = findDepthFormat(); 
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT; 
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; 
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //wont be used after draw 
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; 
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; 
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //dont care about previous depth contents 
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; 

    VkAttachmentReference depthAttachmentRef{}; 
    depthAttachmentRef.attachment = 1; 
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; 

    /* Color attachment */
    VkAttachmentDescription colorAttachment{};
    //format of color attachment needs to match the swapChain image format
    colorAttachment.format = swapChainImageFormat;
    //no multisampling needed so leave at 1 samples
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    /* Choices for loadOp:
        1. VK_ATTACHMENT_LOAD_OP_LOAD: preserve the existing contents of the attachment
        2. VK_ATTACHMENT_LOAD_OP_CLEAR: clear the values to a constant at the start
        3. VK_ATTACHMENT_LOAD_OP_DONT_CARE: existing contents are undefined
    */
    //what do to with data before rendering
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    /* Choices for storeOp:
        1. VK_ATTACHMENT_STORE_OP_STORE: rendered contents will be stored for later use
        2. VK_ATTACHMENT_STORE_OP_DONT_CARE: contents of the frame buffer will be undefined after the rendering operation
    */
    //what to do with data after rendering
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //since we want to see the rendered triangle, we are going to store 

    /*Image layouts can change depending on what operation is being performed
    * Possible layouts are:
    * 1. VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: image is used as a color attachment
    * 2. VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: images to be presented in the swap chain
    * 3. VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: images to be used as destination for memory copy operation
    */
    //dont care what format image is in before render - contents of image are not guaranteed to be preserved 
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //want image to be ready for display 
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    /* Color attachment references */
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; //will give best performance

    /* Subpass */
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef; 

    /* Subpass Dependencies */
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //allow for write 

    /* Render Pass */
    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment }; 
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size()); 
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass");
    }
}

void HelloSquareApplication::createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    //iterate through each image and create a buffer for it 
    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {
            swapChainImageViews[i],
            depthImageView //same depth image is going to be used for all swap chain images 
        }; 

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        //make sure that framebuffer is compatible with renderPass (same # and type of attachments)
        framebufferInfo.renderPass = renderPass;
        //specify which vkImageView objects to bind to the attachment descriptions in the render pass pAttachment array
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size()); 
        framebufferInfo.pAttachments = attachments.data(); 
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1; //# of layers in image arrays

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer");
        }
    }
}

void HelloSquareApplication::createCommandPools() {
    QueueFamilyIndices queueFamilyIndicies = findQueueFamilies(physicalDevice);

    /* Command Buffers */
    //command buffers must be submitted on one of the device queues (graphics or presentation queues in this case)
    //must only be submitted on a single type of queue
    //creating commands for drawing, as such these are submitted on the graphics family 
    /* Two possible flags for command pools:
        1.VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: warn vulkan that the command pool is changed often
        2.VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: allow command buffers to be rerecorded individually, without this all command buffers are reset at the same time
    */
    //commandPoolInfo.flags = 0; //optional -- will not be changing or resetting any command buffers 

    //graphics command buffer
    createPool(queueFamilyIndicies.graphicsFamily.value(), 0, graphicsCommandPool);

    //command buffer for transfer queue 
    createPool(queueFamilyIndicies.transferFamily.value(), 0, transferCommandPool);

    //temporary command pool --unused at this time
    //createPool(queueFamilyIndicies.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, tempCommandPool); 

}

void HelloSquareApplication::createTextureImage()
{
    int texWidth, texHeight, texChannels; 

    stbi_uc* pixels = stbi_load("media/images/texture.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha); 
    VkDeviceSize imageSize = texWidth * texHeight * 4; 

    if (!pixels) {
        throw std::runtime_error("Failed to load texture image!"); 
    }

    /* Create Staging Buffer */
    VkBuffer stagingBuffer; 
    VkDeviceMemory stagingBufferMemory; 

    //buffer needs to be in host visible memory
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory); 

    //copy over to staging buffer
    void* data; 
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data); 
    memcpy(data, pixels, static_cast<size_t>(imageSize)); 
    vkUnmapMemory(device, stagingBufferMemory); 

    //free up image in stbi 
    stbi_image_free(pixels); 

    createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory); 

    //copy staging buffer to texture image 
    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)); 

    //prepare final image for texture mapping in shaders 
    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); 

    vkDestroyBuffer(device, stagingBuffer, nullptr); 
    vkFreeMemory(device, stagingBufferMemory, nullptr); 
}

void HelloSquareApplication::createTextureImageView()
{
    textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

void HelloSquareApplication::createTextureSampler()
{
    //get device properties for amount of anisotropy permitted
    VkPhysicalDeviceProperties deviceProperties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    VkSamplerCreateInfo samplerInfo{}; 
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO; 
    samplerInfo.magFilter = VK_FILTER_LINEAR;                       //how to sample textures that are magnified 
    samplerInfo.minFilter = VK_FILTER_LINEAR;                       //how to sample textures that are minified
    
    
    //repeat mode - repeat the texture when going beyond the image dimensions
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; 
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    //should anisotropic filtering be used? Really only matters if performance is a concern
    samplerInfo.anisotropyEnable = VK_TRUE; 
    //specifies the limit on the number of texel samples that can be used (lower = better performance)
    samplerInfo.maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy; 
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; 
    //specifies coordinate system to use in addressing texels. 
        //VK_TRUE - use coordinates [0, texWidth) and [0, texHeight]
        //VK_FALSE - use [0, 1)
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    //if comparing, the texels will first compare to a value, the result of the comparison is used in filtering operations (percentage-closer filtering on shadow maps)
    samplerInfo.compareEnable = VK_FALSE; 
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    //following apply to mipmapping -- not using here
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; 
    samplerInfo.mipLodBias = 0.0f; 
    samplerInfo.minLod = 0.0f; 
    samplerInfo.maxLod = 0.0f; 

    if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!"); 
    }
}

void HelloSquareApplication::createDepthResources()
{
    //depth image should have:
    //  same resolution as the color attachment (in swap chain extent)
    //  optimal tiling and device local memory 
    //Need to decide format - need to decide format for the accuracy since no direct access to the depth image from CPU side 
    //Formats for color image: 
    //  VK_FORMAT_D32_SFLOAT: 32-bit-float
    //  VK_FORMAT_D32_SFLOAT_S8_UINT: 32-bit signed float for depth and 8 bit stencil component
    //  VK_FORMAT_D24_UNFORM_S8_UINT: 24-bit float for depth and 8 bit stencil component

    VkFormat depthFormat = findDepthFormat(); 

    createImage(swapChainExtent.width, swapChainExtent.height, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory); 
    depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void HelloSquareApplication::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    /* Create vulkan image */
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image");
    }

    /* Allocate the memory for the imag*/
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device, image, imageMemory, 0);
}

VkImageView HelloSquareApplication::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo{}; 
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; 
    viewInfo.image = image; 
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; 
    viewInfo.format = format; 
    viewInfo.subresourceRange.aspectMask = aspectFlags; 
    viewInfo.subresourceRange.baseMipLevel = 0; 
    viewInfo.subresourceRange.levelCount = 1; 
    viewInfo.subresourceRange.baseArrayLayer = 0; 
    viewInfo.subresourceRange.layerCount = 1; 
    
    VkImageView imageView; 
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!"); 
    }

    return imageView; 
}

bool HelloSquareApplication::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT; 
}

VkFormat HelloSquareApplication::findDepthFormat()
{
    //utilizin g the VK_FORMAT_FEATURE_ flag to check for candidates that have a depth component.
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT); 

}

VkFormat HelloSquareApplication::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props; 
        //VkFormatProperties: 
            //linearTilingFeatures
            //optimalTilingFeatures
            //bufferFeatures
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);


        //check if the properties matches the requirenments for tiling
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if ((tiling == VK_IMAGE_TILING_OPTIMAL) && (props.optimalTilingFeatures & features) == features) {
            return format; 
        }
    }

    throw std::runtime_error("failed to find supported format!"); 
}

void HelloSquareApplication::createPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags, VkCommandPool& pool)
{
    QueueFamilyIndices queueFamilyIndicies = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
    commandPoolInfo.flags = flags;

    if (vkCreateCommandPool(device, &commandPoolInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("unable to create pool");
    }
}


void HelloSquareApplication::createCommandBuffers() {
    /* Graphics Command Buffer */
    graphicsCommandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = graphicsCommandPool;
    // .level - specifies if the allocated command buffers are primay or secondary
    // ..._PRIMARY : can be submitted to a queue for execution, but cannot be called from other command buffers
    // ..._SECONDARY : cannot be submitted directly, but can be called from primary command buffers (good for reuse of common operations)
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)graphicsCommandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, graphicsCommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers");
    }

    /* Begin command buffer recording */
    for (size_t i = 0; i < graphicsCommandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        //flags parameter specifies command buffer use 
            //VK_COMMAND_BUFFER_USEAGE_ONE_TIME_SUBMIT_BIT: command buffer recorded right after executing it once
            //VK_COMMAND_BUFFER_USEAGE_RENDER_PASS_CONTINUE_BIT: secondary command buffer that will be within a single render pass 
            //VK_COMMAND_BUFFER_USEAGE_SIMULTANEOUS_USE_BIT: command buffer can be resubmitted while another instance has already been submitted for execution
        beginInfo.flags = 0;

        //only relevant for secondary command buffers -- which state to inherit from the calling primary command buffers 
        beginInfo.pInheritanceInfo = nullptr;

        /* NOTE:
            if the command buffer has already been recorded once, simply call vkBeginCommandBuffer->implicitly reset.
            commands cannot be added after creation
        */

        if (vkBeginCommandBuffer(graphicsCommandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        /* Begin render pass */
        //drawing starts by beginning a render pass 
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

        //define the render pass we want
        renderPassInfo.renderPass = renderPass;

        //what attachments do we need to bind
        //previously created swapChainbuffers to hold this information 
        renderPassInfo.framebuffer = swapChainFramebuffers[i];

        //define size of render area -- should match size of attachments for best performance
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChainExtent;

        //size of clearValues and order, should match the order of attachments
        std::array<VkClearValue, 2> clearValues{}; 
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} }; //clear color for background color will be used with VK_ATTACHMENT_LOAD_OP_CLEAR
        //depth values: 
            //0.0 - closest viewing plane 
            //1.0 - furthest possible depth
        clearValues[1].depthStencil = { 1.0, 0 };  

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size()); 
        renderPassInfo.pClearValues = clearValues.data(); 

        /* vkCmdBeginRenderPass */
        //Args: 
            //1. command buffer to set recording to 
            //2. details of the render pass
            //3. how drawing commands within the render pass will be provided
                //OPTIONS: 
                    //VK_SUBPASS_CONTENTS_INLINE: render pass commands will be embedded in the primary command buffer. No secondary command buffers executed 
                    //VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: render pass commands will be executed from the secondary command buffers
        vkCmdBeginRenderPass(graphicsCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        /* Drawing Commands */
        //Args: 
            //2. compute or graphics pipeline
            //3. pipeline object
        vkCmdBindPipeline(graphicsCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkBuffer vertexBuffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };

        //bind vertex buffers -> how to pass information to the vertex shader once it is uploaded to the GPU
        vkCmdBindVertexBuffers(graphicsCommandBuffers[i], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(graphicsCommandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32); 

        /* vkCmdBindDescriptorSets: 
        *   1. 
        *   2. Descriptor sets are not unique to graphics pipeliens, must specify to use in graphics or compute pipelines. 
        *   3. layout that the descriptors are based on 
        *   4. index of first descriptor set 
        *   5. number of sets to bind 
        *   6. array of sets to bind 
        *   7 - 8. array of offsets used for dynamic descriptors (not used here)
        */
        //bind the right descriptor set for each swap chain image to the descripts in the shader 
        vkCmdBindDescriptorSets(graphicsCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr); 

        //now create call to draw
        //Args:    
            //2. vertexCount: how many verticies to draw
            //3. instanceCount: used for instanced render, use 1 otherwise
            //4. firstVertex: offset in VBO, defines lowest value of gl_VertexIndex
            //5. firstInstance: offset for instanced rendering, defines lowest value of gl_InstanceIndex -> not using so leave at 1 
        //TODO: currently drawing ALL verticies...might need to make more flexible with more objects
        vkCmdDrawIndexed(graphicsCommandBuffers[i], static_cast<uint32_t>(numIndicies), 1, 0, 0, 0);
        //vkCmdDrawIndexed(graphicsCommandBuffers[i], 3, 1, 0, 0, 0); 

        //can now finish render pass
        vkCmdEndRenderPass(graphicsCommandBuffers[i]);

        //record command buffer
        if (vkEndCommandBuffer(graphicsCommandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }
    }

    /* Transfer Command Buffer */
    //transferCommandBuffers.resize(swapChainFramebuffers.size()); 
    //VkCommandBufferAllocateInfo transferAllocInfo{}; 
    //transferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; 
    //transferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; 
    //transferAllocInfo.commandPool = transferCommandPool; 
    //allocInfo.commandBufferCount = 1; 

    //if (vkAllocateCommandBuffers(device, &transferAllocInfo, &transferCommandBuffers)); 

}

void HelloSquareApplication::createSemaphores() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS || vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i])) {
            throw std::runtime_error("failed to create semaphores for a frame");
        }
    }
}

void HelloSquareApplication::createFences() {
    //note: fence creation can be rolled into semaphore creation. Seperated for understanding
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    //create the fence in a signaled state 
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create fence object for a frame");
        }
    }
}

void HelloSquareApplication::createFenceImageTracking() {
    //note: just like createFences() this too can be wrapped into semaphore creation. Seperated for understanding.

    //need to ensure the frame that is going to be drawn to, is the one linked to the expected fence.
    //If, for any reason, vulkan returns an image out of order, we will be able to handle that with this link
    imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

    //initially, no frame is using any image so this is going to be created without an explicit link
}

void HelloSquareApplication::createVertexBuffer() { 
    VulkanObject* currentObject = nullptr; 
    VkDeviceSize bufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    //TODO: ensure that more objects can be drawn 

    //go through all objects in object list and generate the vertex indicies -- only works with 1 object at the moment 
    currentObject = &this->objectList->at(0); 
    auto verticies = currentObject->GetVerticies(); 
    bufferSize = sizeof(verticies[0]) * verticies.size();

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);


    /* Filling the vertex buffer */
    void* data;

    //access a region of the specified memory resource defined by an offset and size 
    //can also specify VK_WHOLE_SIZE to map all memory 
    //currrently no memory flags available in API (time of writing) so must be set to 0
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, verticies.data(), (size_t)bufferSize); //simply copy data into mapped memory
    vkUnmapMemory(device, stagingBufferMemory); //unmap memory

    /* Staging Buffer */
    //New flags 
    //VK_BUFFER_USAGE_TRANSFER_SRC_BIT: buffer can be used as source in a memory transfer 
    //VK_BUFFER_USAGE_TRANSFER_DST_BIT: buffer can be used as destination in a memory transfer 
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize); //actually call to copy memory

    //cleanup 
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    /* Memory Copy Note */
    //note: driver might not immediately copy the data into the buffer memory, ex: caching
    //also possible that writes not visible to mapped memory yet
    //two ways to handle this: 
        //1. use memory heap that is host coherent (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) -- memory is matched to the GPU
        //2. call vkFlushMappedMemoryRanges after writing tot he mapped memory and then call vkInvalidateMappedMemoryRanges before reading from mapped memory
    //this project used (1) memory always matches but might lead to slightly worse performance 
    //Flushing memory ranges or using coherent calls does not mean they are passed to GPU. Vulkan does this in the background and memory is guaranteed to be on
        //on GPU before next call to vkQueueSubmit
}

void HelloSquareApplication::createIndexBuffer()
{
    VkDeviceSize bufferSize; 
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    //TODO: will only support one object at the moment

    VulkanObject* currObject = &objectList->at(0); 
    std::vector<uint32_t> indicies = currObject->GetIndicies(); 

    bufferSize = sizeof(indicies[0]) * indicies.size();
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indicies.data(), (size_t)bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    //note the use of VK_BUFFER_USAGE_INDEX_BUFFER_BIT due to the use of the indicies
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

    copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

}

void HelloSquareApplication::createRenderingBuffers()
{
    /*Create Uniform Buffer */
    //just set up the buffers, we are not going to be updating the buffers here since they need updated every frame
    VkDeviceSize uboBufferSize = sizeof(UniformBufferObject); 

    uniformBuffers.resize(swapChainImages.size()); 
    uniformBuffersMemory.resize(swapChainImages.size()); 

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        createBuffer(uboBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
    }
}

void HelloSquareApplication::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage; //purpose of data in buffer
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //buffers can be owned by specific queue family or shared between them at the same time. This only used for graphics queue

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer");
    }

    //need to allocate memory for the buffer object
    /* VkMemoryRequirements:
        1. size - number of required bytes in memory
        2. alignments - offset in bytes where the buffer begins in the allocated region of memory (depends on bufferInfo.useage and bufferInfo.flags)
        3. memoryTypeBits - bit fied of the memory types that are suitable for the buffer
    */
    VkMemoryRequirements memRequirenments;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirenments);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirenments.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirenments.memoryTypeBits, properties);

    //should not call vkAllocateMemory for every object. Bundle objects into one call and use offsets 
    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory");
    }

    //4th argument: offset within the region of memory. Since memory is allocated specifically for this vertex buffer, the offset is 0
    //if not 0, required to be divisible by memRequirenments.alignment
    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void HelloSquareApplication::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    bool useTransferPool = true; 
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(useTransferPool);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;

    //note: cannot specify VK_WHOLE_SIZE as before 
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer, useTransferPool);
}

void HelloSquareApplication::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    bool useTransferPool = true; 

    VkCommandBuffer commandBuffer = beginSingleTimeCommands(useTransferPool);

    //specify which region of the buffer will be copied to the image 
    VkBufferImageCopy region{}; 
    region.bufferOffset = 0;                                            //specifies byte offset in the buffer at which the pixel values start
    //the following specify the layout of pixel information in memory
    region.bufferRowLength = 0; 
    region.bufferImageHeight = 0; 

    //the following indicate what part of the image we want to copy to 
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; 
    region.imageSubresource.mipLevel = 0; 
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1; 
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    //enque copy operation 
    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,   //assuming image is already in optimal format for copy operations
        1,
        &region
    ); 

    endSingleTimeCommands(commandBuffer, useTransferPool); 
}

void HelloSquareApplication::updateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now(); 

    auto currentTime = std::chrono::high_resolution_clock::now(); 
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count(); 

    //create new ubo object which will create transformation around the Z axis 
    UniformBufferObject ubo{}; 

    //glm::mat4(1,0f) = identity matrix
    //time * radians(90) = rotate 90degrees per second
    if (spin) {
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    else {
        ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    //ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); 

    //look at geometry from above at 45 degree angle 
    /* LookAt takes:
    *   1. eye position
    *   2. center position
    *   3. center position 
    *   4. up axis
    */
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); 

    //perspective projection with 45 degree vertical field of view -- important to use swapChainExtent to calculate aspect ratio (REFRESH WITH WINDOW RESIZE)
    /* perspective takes: 
    *   1. fov 
    *   2. aspect ratio
    *   3. near view plane 
    *   4. far view plane 
    */
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f); 
    
    //glm designed for openGL where the Y coordinate of the flip coordinates is inverted. Fix this by flipping the sign on the scaling factor of the Y axis in the projection matrix.
    ubo.proj[1][1] *= -1; 

    //copy data to the current uniform buffer 
    void* data; 
    vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data); 
    memcpy(data, &ubo, sizeof(ubo)); 
    vkUnmapMemory(device, uniformBuffersMemory[currentImage]); 
}


void HelloSquareApplication::createDescriptorPool()
{
    //descriptor sets cant be created directly, they must be allocated from a pool like command buffers. 
    VkDescriptorPoolSize descriptorPoolSize{};

    std::array<VkDescriptorPoolSize, 2> poolSizes{}; 

    /* Uniform Buffers : 1 for scene and 1 per object */
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());     //allocate a descriptor for each frame
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; 
    poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

    VkDescriptorPoolCreateInfo poolInfo{}; 
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO; 
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size()); 

    //must also define max number of descriptor sets that are available 
    poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size()); 

    //can leave flags at 0, can use VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT to allow for changing a descriptor set after creation
    poolInfo.flags = 0; 

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool"); 
    }
    
}

void HelloSquareApplication::createDescriptorSets()
{
    //describe the descriptor set layouts to vulkan when allocating memory
    std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{}; 
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO; 
    allocInfo.descriptorPool = descriptorPool; 
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size()); 
    allocInfo.pSetLayouts = layouts.data(); 

    descriptorSets.resize(swapChainImages.size()); 

    //make call to allocate memory
    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets");
    }

    //apply information to each of the previously allocated descriptor set 
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkDescriptorBufferInfo uboBufferInfo{}; 
        uboBufferInfo.buffer = uniformBuffers[i];
        uboBufferInfo.offset = 0;
        uboBufferInfo.range = sizeof(UniformBufferObject);

        //create pool for texture and texture sampler
        VkDescriptorImageInfo imageInfo{}; 
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; 
        imageInfo.imageView = textureImageView; 
        imageInfo.sampler = textureSampler; 

        std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{}; 
        /* Binding 0: Uniform Buffer */
        writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[0].dstSet = descriptorSets[i];      //which descriptor set to update 
        writeDescriptorSets[0].dstBinding = 0;                  //which binding to update - the UBO was previously given an index of 0
        writeDescriptorSets[0].dstArrayElement = 0;             //descriptors can be arrays, specifiy first index of array to update - not using array here 

        writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSets[0].descriptorCount = 1;             //possible to update more than one descriptor in an array, starting at index dstArrayElement
        
        //reference array with descriptorCount and actually configures the descriptor. Depends on the type of descriptor which should be used. 
        writeDescriptorSets[0].pBufferInfo = &uboBufferInfo;    //used for descriptors that refer to buffer data 
        writeDescriptorSets[0].pImageInfo = nullptr;            //used for descriptors that refer to image data 
        writeDescriptorSets[0].pTexelBufferView = nullptr;      //used for descriptors that refer to buffer views
        
        /* Binding 1: Texture Attributes */
        writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; 
        writeDescriptorSets[1].dstSet = descriptorSets[i]; 
        writeDescriptorSets[1].dstBinding = 1; 
        writeDescriptorSets[1].dstArrayElement = 0; 
        writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; 
        writeDescriptorSets[1].descriptorCount = 1; 
        writeDescriptorSets[1].pImageInfo = &imageInfo;         //ref to image that will be allocated to this pool

        //make actual call to vulkan to update the descriptor sets
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr); //can also take a vkCopyDescriptorSet -> can be used to copy descriptors to each other
    }

    /*NOTE: descriptor sets do not need to be explicitly destroyed since they will be cleaned when the descriptor pool is destroyed*/
}

void HelloSquareApplication::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(); 

    //create a barrier to prevent pipeline from moving forward until image transition is complete
    VkImageMemoryBarrier barrier{}; 
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;     //specific flag for image operations
    barrier.oldLayout = oldLayout; 
    barrier.newLayout = newLayout; 

    //if barrier is used for transferring ownership between queue families, this would be important -- set to ignore since we are not doing this
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; 
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image; 
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; 
    barrier.subresourceRange.baseMipLevel = 0;                          //image does not have any mipmap levels
    barrier.subresourceRange.levelCount = 1;                            //image is not an array
    barrier.subresourceRange.baseArrayLayer = 0; 
    barrier.subresourceRange.layerCount = 1;

    //the operations that need to be completed before and after the barrier, need to be defined
    barrier.srcAccessMask = 0; //TODO
    barrier.dstAccessMask = 0; //TODO

    VkPipelineStageFlags sourceStage, destinationStage; 

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        //undefined transition state, dont need to wait for this to complete
        barrier.srcAccessMask = 0; 
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; 

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; 
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT; 
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        //transfer destination shdaer reading, will need to wait for completion. Especially in the frag shader where reads will happen
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; 
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT; 

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT; 
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; 
    }
    else {
        throw std::invalid_argument("unsupported layout transition!"); 
    }

    //transfer writes must occurr during the pipeline transfer stage
    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,                    //which pipeline stages should occurr before barrier 
        destinationStage,               //pipeline stage in which operations iwll wait on the barrier 
        0,                  
        0, 
        nullptr,
        0, 
        nullptr,
        1, 
        &barrier
    ); 


    endSingleTimeCommands(commandBuffer); 
}

VkCommandBuffer HelloSquareApplication::beginSingleTimeCommands(bool useTransferPool)
{
    //allocate using temporary command pool
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = useTransferPool ? transferCommandPool : graphicsCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer tmpCommandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &tmpCommandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; //only planning on using this command buffer once 

    vkBeginCommandBuffer(tmpCommandBuffer, &beginInfo);

    return tmpCommandBuffer;
}

void HelloSquareApplication::endSingleTimeCommands(VkCommandBuffer commandBuffer, bool useTransferPool)
{
    vkEndCommandBuffer(commandBuffer); 

    //submit the buffer for execution
    VkSubmitInfo submitInfo{}; 
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; 
    submitInfo.commandBufferCount = 1; 
    submitInfo.pCommandBuffers = &commandBuffer; 

    if (useTransferPool){
        vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(transferQueue);
        vkFreeCommandBuffers(device, transferCommandPool, 1, &commandBuffer);
    }
    else {
        //use graphics pool
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);
        vkFreeCommandBuffers(device, graphicsCommandPool, 1, &commandBuffer);
    }
}


#pragma region Unused Functions
/* LEFT FOR FUTURE NOTE
/// <summary>
/// finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
/// finalColor.a = newAlpha.a;
/// </summary>
/// <returns></returns>
VkPipelineColorBlendAttachmentState HelloSquareApplication::createAlphaColorBlending() {
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}
*/
#pragma endregion
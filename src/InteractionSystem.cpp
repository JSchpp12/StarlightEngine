#include "InteractionSystem.h"

std::vector<std::unique_ptr<std::function<void(int, int, int, int)>>> star::InteractionSystem::keyboardCallbacks	= std::vector<std::unique_ptr<std::function<void(int, int, int, int)>>>();
std::vector<std::unique_ptr<std::function<void(double, double)>>> star::InteractionSystem::mouseMovementCallbacks	= std::vector<std::unique_ptr<std::function<void(double, double)>>>();
std::vector<std::unique_ptr<std::function<void(int, int, int)>>> star::InteractionSystem::mouseButtonCallbacks		= std::vector<std::unique_ptr<std::function<void(int, int, int)>>>(); 
std::vector<std::unique_ptr<std::function<void(double, double)>>> star::InteractionSystem::mouseScrollCallbacks		= std::vector<std::unique_ptr<std::function<void(double, double)>>>();
std::vector<std::unique_ptr<std::function<void()>>> star::InteractionSystem::worldUpdateCallbacks					= std::vector<std::unique_ptr<std::function<void()>>>(); 

void star::InteractionSystem::registerKeyCallback(std::unique_ptr<std::function<void(int, int, int, int)>> newKeyCallback)
{
	keyboardCallbacks.push_back(std::move(newKeyCallback));
}

void star::InteractionSystem::registerMouseMovementCallback(std::unique_ptr<std::function<void(double, double)>> newMouseMvmtCallback)
{
	mouseMovementCallbacks.push_back(std::move(newMouseMvmtCallback));
}

void star::InteractionSystem::registerMouseButtonCallback(std::unique_ptr<std::function<void(int, int, int)>> newMouseButtonCallback)
{
	mouseButtonCallbacks.push_back(std::move(newMouseButtonCallback)); 
}

void star::InteractionSystem::registerMouseScrollCallback(std::unique_ptr<std::function<void(double, double)>> newMouseScrollCallback)
{
	mouseScrollCallbacks.push_back(std::move(newMouseScrollCallback)); 
}

void star::InteractionSystem::registerWorldUpdateCallback(std::unique_ptr<std::function<void()>> newWorldUpdateCallback)
{
	worldUpdateCallbacks.push_back(std::move(newWorldUpdateCallback)); 
}

void star::InteractionSystem::glfwKeyHandle(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	std::function<void(int, int, int, int)> call; 
	//call all registered functions
	for (size_t i = 0; i < keyboardCallbacks.size(); i++) {
		call = *keyboardCallbacks.at(i);
		call(key, scancode, action, mods);
	}
}

void star::InteractionSystem::glfwMouseMovement(GLFWwindow* window, double xpos, double ypos)
{
	std::function<void(double, double)> call;

	for (size_t i = 0; i < mouseMovementCallbacks.size(); i++) {
		call = *mouseMovementCallbacks.at(i); 
		call(xpos, ypos); 
	}
}

void star::InteractionSystem::glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	std::function<void(int, int, int)> call; 

	for (size_t i = 0; i < mouseButtonCallbacks.size(); i++) {
		call = *mouseButtonCallbacks.at(i); 
		call(button, action, mods); 
	}
}

void star::InteractionSystem::glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	std::function<void(double, double)> call;

	for (size_t i = 0; i < mouseScrollCallbacks.size(); i++) {
		call = *mouseScrollCallbacks.at(i); 
		call(xoffset, yoffset);
	}
}

void star::InteractionSystem::callWorldUpdates()
{
	std::function<void()> call; 

	for (size_t i = 0; i < worldUpdateCallbacks.size(); i++) {
		call = *worldUpdateCallbacks.at(i); 
		call(); 
	}
}

#include "OptionsController.h"

namespace star {
	OptionsController::OptionsController(common::RenderOptions& renderOptions): renderOptions(renderOptions)
	{
		this->printHelp(); 
	}
	void OptionsController::keyCallback(int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_0) {
				std::cout << "Enabling standard draw" << std::endl; 
				renderOptions.setRenderMaterialSetting(common::Render_Settings::none);
			}
			else if (key == GLFW_KEY_1) {
				std::cout << "Enabling render material option: draw ambient property" << std::endl; 
				renderOptions.setRenderMaterialSetting(common::Render_Settings::ambient);
			}
		}
	}
	void OptionsController::printHelp()
	{
		std::cout << "Default options controller has been enabled..." << std::endl;
		std::cout << "Controls: " << std::endl;
		std::cout << "NUM: 0 - Normal Draw" << std::endl;
		std::cout << "NUM: 1 - Draw Ambient Properties" << std::endl;
	}
}
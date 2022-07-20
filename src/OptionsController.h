#pragma once 
#include "SC/Interactivity.hpp"
#include "SC/RenderOptions.hpp"
#include "SC/Enums.h"

#include <GLFW/glfw3.h>

#include <iostream>

namespace star{
	class OptionsController : public common::Interactivity {
	public:
		OptionsController(common::RenderOptions& renderOptions);
		virtual void keyCallback(int key, int scancode, int action, int mods) override;

	private:
		common::RenderOptions& renderOptions; 
		static void printHelp(); 
	};
}
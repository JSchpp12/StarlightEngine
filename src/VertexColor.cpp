#include "VertexColor.h"

typedef std::chrono::high_resolution_clock Clock;

VertexColor::VertexColor()
{
	//random time period between 5 and 30 
	changeTempo = static_cast<long long> (rand()) % 20 + 150;

	RGB_tracker = static_cast<int> (rand() % 2 + 1); 

	//init color attributes
	for (auto& color : colors) {
		color.setValue(static_cast<float> (rand()) / static_cast<float> (RAND_MAX)); 
		color.isValueIncreasing = ((rand() % 2 + 1) == 1);
	}
}

VertexColor::~VertexColor()
{
}

glm::vec3 VertexColor::GetVertexColor()
{
	return glm::vec3(colors[0].Value(), colors[1].Value(), colors[2].Value());
}

bool VertexColor::IsTimeToUpdate(long long inDuration)
{
	changeTracker += inDuration; 
	if (changeTracker > changeTempo) {
		changeTracker = 0; 
		return true; 
	}
	return false;
}

void VertexColor::UpdateColor(float changeAmmt)
{
	lastUpdateTime = std::chrono::high_resolution_clock::now(); 

	if (colors[RGB_tracker].isValueIncreasing) {
		colors[RGB_tracker].setValue(colors[RGB_tracker].Value() + changeAmmt);

		//flip direction of color when it reaches a max value
		if (colors[RGB_tracker].Value() >= 1.0f) {
			colors[RGB_tracker].isValueIncreasing = false;
			incrementRGBTracker(); 
		}
	}
	else {
		colors[RGB_tracker].setValue(colors[RGB_tracker].Value() - changeAmmt);

		//flip direction
		if (colors[RGB_tracker].Value() <= 0.0f) {
			colors[RGB_tracker].isValueIncreasing = true;
			incrementRGBTracker();
		}
	}
}

void VertexColor::SetColor(glm::vec3 newColor)
{
	int counter = 0; 

	for (auto& color : colors) {
		color.setValue(newColor[counter++]);
	}
}

void VertexColor::incrementRGBTracker()
{
	if (RGB_tracker == 2) {
		RGB_tracker = 0; 
	}
	else {
		RGB_tracker++; 
	}
}

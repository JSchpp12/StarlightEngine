#pragma once
#include <glm/glm.hpp>
#include <chrono>
#include <stdlib.h>


class VertexColor
{
public:

	/// <summary>
	/// Create a color for a vertex with random values
	/// </summary>
	VertexColor();
	~VertexColor();


	glm::vec3 GetVertexColor(); 
	bool IsTimeToUpdate(long long inDuration); 
	void UpdateColor(float changeAmmt);
	void SetColor(glm::vec3 newColor); 


private:
	std::chrono::steady_clock::time_point lastUpdateTime, updateCounter;
	long long changeTempo, changeTracker = 0;
	int RGB_tracker; 

	struct Color {
		bool isValueIncreasing; //will be true if the color value is increasing

		float Value() {
			return value; 
		}

		void setValue(float newVal) {
			if (newVal >= 1.0f) {
				value = 1.0f; 
				return; 
			}
			if (newVal <= 0.0f) {
				value = 0.0f;
				return; 
			}
			value = newVal; 
		}

		private: 
			float value; 
	};
	Color colors[3]; 

	void incrementRGBTracker(); 
};
#pragma once 
#include "SC/Camera.hpp"
#include "SC/Interactivity.hpp"
#include "SC/Time.hpp"

#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

namespace star {
	/// <summary>
	/// Camera with default controls
	/// </summary>
	class CameraController : 
		public common::Camera, 
		public common::Interactivity
	{
	public:
		/// <summary>
		/// Key callback for camera object. Implements default controls for the camera. 
		/// </summary>
		/// <param name="key"></param>
		/// <param name="scancode"></param>
		/// <param name="action"></param>
		/// <param name="mods"></param>
		virtual void keyCallback(int key, int scancode, int action, int mods) override;

		/// <summary>
		/// Mouse callback for camera objects. Implements default controls for the camera. 
		/// </summary>
		/// <param name="xpos"></param>
		/// <param name="ypos"></param>
		virtual void mouseMovementCallback(double xpos, double ypos) override;

		/// <summary>
		/// Mouse button callback for camera object. 
		/// </summary>
		/// <param name="button"></param>
		/// <param name="action"></param>
		/// <param name="mods"></param>
		virtual void mouseButtonCallback(int button, int action, int mods) override; 

		/// <summary>
		/// Update camera locations as needed 
		/// </summary>
		virtual void worldUpdate() override;

	protected:

	private: 
		const float sensitivity = 0.1f; 
		bool init = false; 

		//previous mouse coordinates from GLFW
		float prevX, prevY, xMovement, yMovement; 

		//control information for camera 
		float pitch=-0.f, yaw=-90.0f; 

		bool click = false;
		bool moveLeft, moveRight = false, moveForward = false, moveBack = false; 
	};
}
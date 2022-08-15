#include "CameraController.h"

void star::CameraController::keyCallback(int key, int scancode, int action, int mods) {
	if (action != GLFW_RELEASE) {
		if (!this->moveForward && key == GLFW_KEY_W) {
			this->moveForward = true;
		}
		if (!this->moveBack && key == GLFW_KEY_S) {
			this->moveBack = true; 
			//cameraPos -= moveAmt * cameraLookDir;
			//this->setPosition(cameraPos);
		}
		if (!this->moveLeft && key == GLFW_KEY_A) {
			this->moveLeft = true; 
		}
		if (!this->moveRight && key == GLFW_KEY_D) {
			this->moveRight = true; 
		}
	}

	if (action == GLFW_RELEASE) {
		if (this->moveLeft && key == GLFW_KEY_A) {
			this->moveLeft = false; 
		}
		if (this->moveRight && key == GLFW_KEY_D) {
			this->moveRight = false; 
		}
		if (this->moveForward && key == GLFW_KEY_W) {
			this->moveForward = false; 
		}
		if (this->moveBack && key == GLFW_KEY_S) {
			this->moveBack = false; 
		}
	}
}

void star::CameraController::mouseMovementCallback(double xpos, double ypos) {
	if (this->click) {
		//prime camera
		if (!this->init) {
			this->prevX = xpos;
			this->prevY = ypos;
			this->init = true;
		}
		this->xMovement = xpos - this->prevX; 
		this->yMovement = ypos - this->prevY; 
		this->prevX = xpos; 
		this->prevY = ypos; 
	}
}

void star::CameraController::mouseButtonCallback(int button, int action, int mods)
{
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		this->click = true; 
	}
	else if (this->click && button == GLFW_MOUSE_BUTTON_LEFT) {
		this->click = false;
		this->init = false;
	}
}

void star::CameraController::worldUpdate() {
	//TODO: improve time var allocation 
	if ((double)time.timeElapsedLastFrameSeconds() > 1) {
		time.updateLastFrameTime();
	}
	if (this->moveLeft || this->moveRight || this->moveForward || this->moveBack) {
		float moveAmt = 0.1f * time.timeElapsedLastFrameSeconds();

		glm::vec3 cameraPos = this->getPosition();
		glm::vec3 cameraLookDir = -this->getLookDirection();

		if (this->moveLeft) {
			this->moveRelative(glm::cross(cameraLookDir, *this->upVector), moveAmt);
		}
		if (this->moveRight) {
			this->moveRelative(glm::cross(cameraLookDir, -*this->upVector), moveAmt);
		}
		if (this->moveForward) {
			this->moveRelative(*this->lookDirection, moveAmt);
		}
		if (this->moveBack) {
			this->moveRelative(-*this->lookDirection, moveAmt);
		}
		//std::cout << cameraPos.x << "," << cameraPos.y << "," << cameraPos.z << std::endl;

		time.updateLastFrameTime();
	}

	if (this->click) {
		this->xMovement *= this->sensitivity;
		this->yMovement *= this->sensitivity;

		this->yaw += this->xMovement; 
		this->pitch += this->yMovement; 

		//apply restrictions due to const up vector for the camera 
		if (this->pitch > 89.0f) {
			pitch = 89.0f;
		}
		if (this->pitch < -89.0f) {
			pitch = -89.0f;
		}

		glm::vec3 direction{
			cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
			sin(glm::radians(this->pitch)),
			sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))
		};
		this->lookDirection = std::make_unique<glm::vec3>(glm::normalize(direction));
	}
}
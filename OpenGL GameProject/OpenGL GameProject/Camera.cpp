#include "Camera.h"
#include "Player.h"

Camera::Camera(Player* _selectedPlayer)
{
	selectedPlayer = _selectedPlayer;

	currentViewType = PERSP_FOLLOW_PLAYER;
	radiusFromPlayer = 5.25f;
	rotationOffsetFromPlayer = 0.0f;
	rotationAngle = 0.0f;
	//camPos = glm::vec3(0.0f, 3.0f, 0.0f);
	//camLookDir = glm::vec3(0.0f, 0.0f, -1.0f);
	camPos = selectedPlayer->GetPosition();
	camPos.y = 1.0f;
	camPos.z += radiusFromPlayer;
	camPos.x += radiusFromPlayer;
	camLookDir = selectedPlayer->GetPosition();
	camUpDir = glm::vec3(0.0f, 1.0f, 0.0f);

	timeElapsed = (GLfloat)0.0f;

	fovY = glm::radians(95.0f);
	cameraSpring = false;
}

Camera::Camera(viewType _viewType)
{
	currentViewType = _viewType;
	selectedPlayer = nullptr;

	camPos = glm::vec3(0.0f, 0.0f, 5.0f);
	camLookDir = glm::vec3(0.0f, 0.0f, -1.0f);
	camUpDir = glm::vec3(0.0f, 1.0f, 0.0f);

	timeElapsed = (GLfloat)0.0f;

	radiusFromPlayer = 5.25f;
	rotationOffsetFromPlayer = 0.0f;
	rotationAngle = 0.0f;

	fovY = glm::radians(95.0f);
	cameraSpring = false;
}

Camera::~Camera()
{
}

void Camera::update(GLfloat _deltaTime)
{
	deltaTime = _deltaTime;

	glm::vec3 pointAhead;

	switch (currentViewType)
	{
	case ORTHO:	// Orthographic cam setting
		view = lookAt(camPos, camPos + camLookDir, camUpDir);
		
		proj = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT, 0.1f, 10000.0f);
		break;

	case PERSP:	// Perspective
		// Cam rotating around point
		// Comment out if unnecessary
		timeElapsed += _deltaTime * 0.075f;
		
		//camPos.y = 1.5f;
		//camPos.x = sin(timeElapsed) *radius;
		//camPos.z = cos(timeElapsed) *radius;
		pointAhead = glm::vec3(camPos.x, camPos.y * 0.5f, camPos.z + 2.0f);

		view = lookAt(camPos, pointAhead, camUpDir);
		proj = glm::perspective(fovY, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		break;

	case PERSP_FOLLOW_PLAYER:	// Perspective but following player object
		if (selectedPlayer != nullptr)
		{
			rotationAngle = Lerpf(rotationAngle, (selectedPlayer->GetRotationAngle() + rotationOffsetFromPlayer), (_deltaTime / 0.15f)); //+90.0f;

			camPos.x = (cos(glm::radians(-rotationAngle)) * radiusFromPlayer) + selectedPlayer->GetPosition().x;
			camPos.z = (sin(glm::radians(-rotationAngle)) * radiusFromPlayer) + selectedPlayer->GetPosition().z;

			if (!cameraSpring)
			{
				camPos.y = Lerpf(camPos.y, selectedPlayer->GetPosition().y + (radiusFromPlayer * 0.5f), (_deltaTime / 0.75f));
			}
			
			
			//camPos.y = selectedPlayer->GetPosition().y + radiusFromPlayer;
			pointAhead = glm::vec3(camPos.x - selectedPlayer->GetPosition().x, camPos.y - selectedPlayer->GetPosition().y, camPos.z - selectedPlayer->GetPosition().z);
			pointAhead = glm::normalize(pointAhead);

			//view = lookAt(camPos, camPos - pointAhead, camUpDir);
			view = lookAt(camPos, selectedPlayer->GetPosition(), camUpDir);
			proj = glm::perspective(fovY, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		}
		break;

	default:
		break;
	}
}

void Camera::SetFollowedPlayer(Player * selectedPlayer)
{
	this->selectedPlayer = selectedPlayer;
	currentViewType = PERSP_FOLLOW_PLAYER;
}

glm::mat4 Camera::GetPVMatrix()
{
	return proj * view;
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return proj;
}

glm::mat4 Camera::GetViewMatrix()
{
	return view;
}

void Camera::SetCamPos(glm::vec3 newPos)
{
	camPos = newPos;
}

glm::vec3 Camera::GetCamPos()
{
	return camPos;
}

void Camera::SetCamLookDir(glm::vec3 newLookDir)
{
	camLookDir = newLookDir;
}

glm::vec3 Camera::GetCamLookDir()
{
	return camLookDir;
}

void Camera::SetCamUpDir(glm::vec3 newUpDir)
{
	camUpDir = newUpDir;
}

glm::vec3 Camera::GetCamUpDir()
{
	return camUpDir;
}

void Camera::SetCamViewType(viewType _viewType)
{
	currentViewType = _viewType;
}

void Camera::IncrementPos(float incX, float incY, float incZ)
{
	camPos.x += incX;
	camPos.y += incY;
	camPos.z += incZ;
}

void Camera::SetFOVY(float newFOV)
{
	fovY = glm::radians(newFOV);
}

float Camera::GetFOVY()
{
	return glm::degrees(fovY);
}

void Camera::SetCameraSpringIsActive(bool active)
{
	cameraSpring = active;
}

bool Camera::GetCameraSpringIsActive()
{
	return cameraSpring;
}

int Camera::GetCamViewType()
{
	return (int)currentViewType;
}
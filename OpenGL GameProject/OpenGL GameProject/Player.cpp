#include "ShaderLoader.h"
#include "Dependencies\soil\SOIL.h"
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"

#include "Player.h"
#include "Camera.h"
#include "Model.h"
#include "CubeMesh.h"

Player::Player(Model* _playerModel)
{
	if (_playerModel != nullptr)
	{
		playerModel = _playerModel;
		currentPossess = MODEL;
	}
}

Player::Player(CubeMesh * _playerCube)
{
	if (_playerCube != nullptr)
	{
		playerCube = _playerCube;
		currentPossess = CUBE;
	}
}

Player::~Player()
{
	
}

void Player::init()
{
	maxLives = 3;
	currentLives = maxLives;

	speed = 5.0f;
	turnSpeed = 180.0f;

	switch (currentPossess)
	{
	case MODEL:
		
		break;
	case CUBE:
		
		break;
	default:
		std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
	
}

void Player::update(GLfloat _deltaTime)
{
	
}

//void Player::SetControlledCam(Camera * camera)
//{
//	controlledCam = camera;
//}

int Player::GetCurrentLives()
{
	return currentLives;
}

void Player::IncrementLives(int _changeInLives)
{
	currentLives += _changeInLives;
}

glm::vec3 Player::GetPosition()
{
	switch (currentPossess)
	{
	case MODEL:
		return playerModel->GetPosition();
		break;
	case CUBE:
		return playerCube->GetPosition();
		break;
	default:
		assert(std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl);
		//std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

glm::vec3 Player::GetRotation()
{
	switch (currentPossess)
	{
	case MODEL:
		return playerModel->GetRotation();
		break;
	case CUBE:
		return playerCube->GetRotation();
		break;
	default:
		assert(std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl);
		//std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

float Player::GetRotationAngle()
{
	switch (currentPossess)
	{
	case MODEL:
		return playerModel->GetRotationAngle();
		break;
	case CUBE:
		return playerCube->GetRotationAngle();
		break;
	default:
		assert(std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl);
		//std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

void Player::SetShininess(float newShininess)
{
	switch (currentPossess)
	{
	case MODEL:
		playerModel->SetShininess(newShininess);
		break;
	case CUBE:
		playerCube->SetShininess(newShininess);
		break;
	default:
		std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

float Player::GetShininess()
{
	switch (currentPossess)
	{
	case MODEL:
		return playerModel->GetShininess();
		break;
	case CUBE:
		return playerCube->GetShininess();
		break;
	default:
		assert(std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl);
		//std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

void Player::IncrementPosition(float _incXBy, float _incYBy, float _incZBy)
{
	switch (currentPossess)
	{
	case MODEL:
		playerModel->IncrementPosition(_incXBy, _incYBy, _incZBy);
		break;
	case CUBE:
		playerCube->IncrementPosition(_incXBy, _incYBy, _incZBy);
		break;
	default:
		std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
	//position.x += _incXBy;
	//position.y += _incYBy;
	//position.z += _incZBy;
}

void Player::IncrementRotation(float _degrees)
{
	switch (currentPossess)
	{
	case MODEL:
		playerModel->IncrementRotation(_degrees);
		break;
	case CUBE:
		playerCube->IncrementRotation(_degrees);
		break;
	default:
		std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

void Player::IncrementScale(float _incXBy, float _incYBy, float _incZBy)
{
	switch (currentPossess)
	{
	case MODEL:
		playerModel->IncrementScale(_incXBy, _incYBy, _incZBy);
		break;
	case CUBE:
		playerCube->IncrementScale(_incXBy, _incYBy, _incZBy);
		break;
	default:
		std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

void Player::SetRotationAngle(float _angle)
{
	switch (currentPossess)
	{
	case MODEL:
		playerModel->SetRotationAngle(_angle);
		break;
	case CUBE:
		playerCube->SetRotationAngle(_angle);
		break;
	default:
		std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

void Player::IncrementPositionVec(glm::vec3 _incrementVec)
{
	switch (currentPossess)
	{
	case MODEL:
		playerModel->IncrementPositionVec(_incrementVec);
		break;
	case CUBE:
		playerCube->IncrementPositionVec(_incrementVec);
		break;
	default:
		std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

void Player::SetPosition(float _XPos, float _YPos, float _ZPos)
{
	switch (currentPossess)
	{
	case MODEL:
		playerModel->SetPosition(_XPos, _YPos, _ZPos);
		break;
	case CUBE:
		playerCube->SetPosition(_XPos, _YPos, _ZPos);
		break;
	default:
		std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

void Player::SetScale(float _XScale, float _YScale, float _ZScale)
{
	switch (currentPossess)
	{
	case MODEL:
		playerModel->SetScale(_XScale / 10, _YScale / 10, _ZScale / 10);
		break;
	case CUBE:
		playerCube->SetScale(_XScale, _YScale, _ZScale);
		break;
	default:
		std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}

void Player::SetNewPossess(Model * newModelPossessed)
{
	if (playerCube != nullptr)
	{
		playerCube = nullptr;
	}
	currentPossess = MODEL;
	playerModel = newModelPossessed;
}

void Player::SetNewPossess(CubeMesh * newCubePossessed)
{
	if (playerModel != nullptr)
	{
		playerModel = nullptr;
	}
	currentPossess = CUBE;
	playerCube = newCubePossessed;
}

glm::vec3 Player::GetScale()
{
	switch (currentPossess)
	{
	case MODEL:
		return playerModel->GetScale();
		break;
	case CUBE:
		return playerCube->GetScale();
		break;
	default:
		std::cout << "PLAYER IS NOT POSSESSING ANY OBJECT" << std::endl;
		break;
	}
}
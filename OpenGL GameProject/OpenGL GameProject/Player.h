#pragma once
#include "ShaderLoader.h"
#include "Dependencies\soil\SOIL.h"
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"

#include <cmath>
#include <math.h>

// Objects able to be possessed
class Model;
class CubeMesh;

class Camera;

class Player
{
public:
	enum ObjectTypePossessed {
		MODEL = 0,
		CUBE = 1
	};

	// Only ONE object type should != nullptr at a time
	Player(Model* _playerModel);
	Player(CubeMesh* _playerCube);
	~Player();

	void init();
	void update(GLfloat deltaTime);

	//void SetControlledCam(Camera* camera);

	int GetCurrentLives();
	void IncrementLives(int _changeInLives);

	glm::vec3 GetPosition();
	glm::vec3 GetRotation();
	float GetRotationAngle();

	void SetShininess(float newShininess);
	float GetShininess();

	void IncrementPosition(float _incXBy, float _incYBy, float _incZBy);
	void IncrementRotation(float _degrees);
	void IncrementScale(float _incXBy, float _incYBy, float _incZBy);

	void SetRotationAngle(float _angle);

	void IncrementPositionVec(glm::vec3 _incrementVec);

	void SetPosition(float _XPos, float _YPos, float _ZPos);
	void SetScale(float _XScale, float _YScale, float _ZScale);

	// Overloaded function to take in any type of object we want to be able to possess
	void SetNewPossess(Model* newModelPossessed);
	void SetNewPossess(CubeMesh* newCubePossessed);

	glm::vec3 GetScale();

protected:
	// Objects able to be possessed by player Control
	Model* playerModel;
	CubeMesh* playerCube;

	ObjectTypePossessed currentPossess;

	//Camera* controlledCam;
	bool rotateLeft;
	bool rotateRight;

	float speed;
	float turnSpeed;

	float distance;
	float newX, newZ;
	glm::vec3 moveVec;

	int maxLives;
	int currentLives;
};
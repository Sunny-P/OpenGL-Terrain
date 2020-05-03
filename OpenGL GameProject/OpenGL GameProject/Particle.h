#pragma once
#include <random>

#include "Dependencies\glew\glew.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include "Utility.h"

class Particle
{
public:
	Particle(glm::vec3 pOrigin, int id, glm::vec3 velocity, float lifeTime, float speed);
	~Particle();

	void Update(float deltaTime);

	glm::vec3 GetPosition();

	void SetOrigin(glm::vec3 newOrigin);

private:
	glm::vec3 velocity;
	glm::vec3 position;
	glm::vec3 origin;

	float elapsedTime;
	int id;
};


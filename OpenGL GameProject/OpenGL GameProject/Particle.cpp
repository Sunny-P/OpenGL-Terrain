#include "Particle.h"


Particle::Particle(glm::vec3 pOrigin, int id, glm::vec3 velocity, float lifeTime, float speed)
{
	origin = pOrigin;
	this->id = id;
	this->velocity = velocity;
	elapsedTime = lifeTime;
}


Particle::~Particle()
{
}

void Particle::Update(float deltaTime)
{
	this->velocity.y += -0.2f * (deltaTime);
	this->position += velocity;
	this->elapsedTime -= (deltaTime * 0.01f);

	if (this->elapsedTime <= 0.0f)
	{
		this->position = this->origin;
		// Y:  RandomFloatInRange(0.0f, 2.0f)
		this->velocity = glm::vec3(0.1f * cos(this->id * deltaTime) + 0.1f * RandomFloat() - 0.125f,
			0.25f + 0.25f * RandomFloat() - 0.125f,
			0.1f * sin(this->id * deltaTime) + 0.1f * RandomFloat() - 0.125f);
		this->elapsedTime = RandomFloat() + 0.125f;
	}
}

glm::vec3 Particle::GetPosition()
{
	return position;
}

void Particle::SetOrigin(glm::vec3 newOrigin)
{
	origin = newOrigin;
}

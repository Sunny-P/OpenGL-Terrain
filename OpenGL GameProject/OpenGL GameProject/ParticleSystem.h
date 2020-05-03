#pragma once
#include <random>

#include "Dependencies\glew\glew.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"

class Camera;
class Particle;

class ParticleSystem
{
public:
	ParticleSystem(glm::vec3 origin, Camera* camera, GLuint tex);
	~ParticleSystem();

	void Init();
	void Update(float deltaTime);
	void Render(GLuint program);

	void SetOrigin(glm::vec3 newOrigin);

private:
	Camera* camera;

	std::vector<Particle*> vecParticles;
	std::vector<glm::vec3> vecPositions;

	GLuint VAO, VBO, tex, program;
	int numParticles;

	glm::vec3 vQuad1, vQuad2;
	glm::vec3 vView;
	glm::mat4 viewMat;
};


#include "ParticleSystem.h"
#include "Camera.h"
#include "Particle.h"
#include "Utility.h"


ParticleSystem::ParticleSystem(glm::vec3 origin, Camera* camera, GLuint tex)
{
	this->camera = camera;
	numParticles = 10000;
	this->tex = tex;

	for (int i = 0; i < numParticles; ++i)
	{
		vecPositions.push_back(glm::vec3(0.0f));	// Initialise position vector
		// Y:  RandomFloatInRange(0.1f, 1.75f)
		Particle* p = new Particle(origin, i, glm::vec3(0.1f * cos(i * 0.0167f) + 0.1f * RandomFloat() - 0.125f, 
												0.25f + 0.25f * RandomFloat() - 0.125f,
												0.1f * sin(i * 0.0167f) + 0.1f * RandomFloat() - 0.125f), RandomFloat() + 0.125f, 0.5f);
		vecParticles.push_back(p);
	}
}


ParticleSystem::~ParticleSystem()
{
	for (int i = 0; i < vecParticles.size(); ++i)
	{
		delete vecParticles.at(i);
	}
	vecParticles.clear();
}

void ParticleSystem::Init()
{
	glGenVertexArrays(1, &VAO); 
	glBindVertexArray(VAO);
	glGenVertexArrays(1, &VBO); 
	glBindBuffer(GL_ARRAY_BUFFER, VBO); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vecPositions.size(), &vecPositions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0); glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0);

}

void ParticleSystem::Update(float deltaTime)
{
	for (int i = 0; i < numParticles; ++i)
	{
		vecParticles[i]->Update(deltaTime);
		vecPositions[i] = vecParticles[i]->GetPosition();
	}

	viewMat = camera->GetViewMatrix(); 
	vView = camera->GetCamLookDir(); 
	vView = glm::normalize(vView);
	vQuad1 = glm::cross(vView, camera->GetCamUpDir()); 
	vQuad1 = glm::normalize(vQuad1);
	vQuad2 = glm::cross(vView, vQuad1); 
	vQuad2 = glm::normalize(vQuad2);
}

void ParticleSystem::Render(GLuint program)
{
	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "vQuad1"), vQuad1.x, vQuad1.y, vQuad1.z);
	glUniform3f(glGetUniformLocation(program, "vQuad2"), vQuad2.x, vQuad2.y, vQuad2.z);

	glUniformMatrix4fv(glGetUniformLocation(program, "vp"), 1, GL_FALSE, glm::value_ptr(camera->GetPVMatrix()));

	glActiveTexture(GL_TEXTURE0); 
	glUniform1i(glGetUniformLocation(program, "Texture"), 0); 
	glBindTexture(GL_TEXTURE_2D, tex);

	//glEnable(GL_BLEND); 
	//glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vecPositions.size(), &vecPositions[0], GL_STATIC_DRAW);

	glBindVertexArray(VAO); 
	glDrawArrays(GL_POINTS, 0, numParticles);

	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
	//glDepthMask(GL_TRUE); 
}

void ParticleSystem::SetOrigin(glm::vec3 newOrigin)
{
	for (int i = 0; i < vecParticles.size(); ++i)
	{
		vecParticles[i]->SetOrigin(newOrigin);
	}
}

#pragma once
#include "ShaderLoader.h"
#include "Dependencies\soil\SOIL.h"
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"

#include <string>

class Camera;

class GeometryModel
{
public:
	GeometryModel(Camera* camera);
	~GeometryModel();

	void Render(GLuint program);

	void SetPosition(glm::vec3 newPosition);

private:
	Camera* camera;

	GLuint VAO, VBO;

	glm::vec3 position;
	glm::vec3 scale;
};


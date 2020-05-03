#include "GeometryModel.h"
#include "Camera.h"


GeometryModel::GeometryModel(Camera* camera)
{
	this->camera = camera;

	position = glm::vec3(0.0f, 0.0f, 0.0f);

	GLfloat points[] = {
		-1.0f, -1.0f, 0.0f,		// Bottom left
		-1.0,   1.0,  0.0f,		// Top Left
		 1.0f,  1.0f, 0.0f, 	// Top right
		 1.0f, -1.0f, 0.0f };	// Bottom right

	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);

	glGenVertexArrays(1, &VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
}


GeometryModel::~GeometryModel()
{
}

void GeometryModel::Render(GLuint program)
{
	glDisable(GL_CULL_FACE);
	glUseProgram(program);

	glm::mat4 model;
	model = glm::translate(model, position);

	glm::mat4 mvp = camera->GetPVMatrix() * model;

	GLint vpLoc = glGetUniformLocation(program, "mvp");
	glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 4);
	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
}

void GeometryModel::SetPosition(glm::vec3 newPosition)
{
	position = newPosition;
}

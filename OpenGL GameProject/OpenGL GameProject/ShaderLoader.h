#pragma once

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>
#include <map>


class ShaderLoader
{
	private:

		std::string ReadShader(char *filename);
		GLuint CreateShader(GLenum shaderType,
			std::string source,
			char* shaderName);

	public:
		ShaderLoader(void);
		~ShaderLoader(void);
		GLuint CreateProgram(char* VertexShaderFilename,
			char* FragmentShaderFilename);
		GLuint CreateProgram(char* VertexShaderFileName, char* FragmentShaderFilename, char* GeometryShaderFilename);

		std::map<char*, GLuint> VertexShaders;
		std::map<char*, GLuint> FragmentShaders;
		std::map<char*, GLuint> GeometryShaders;
		std::map<std::string, GLuint> Programs;
};

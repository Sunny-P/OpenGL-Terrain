#include "ShaderLoader.h" 
#include<iostream>
#include<fstream>
#include<vector>

ShaderLoader::ShaderLoader(void){}
ShaderLoader::~ShaderLoader(void){}


std::string ShaderLoader::ReadShader(char *filename)
{
	std::string shaderCode;
	std::ifstream file(filename, std::ios::in);

	if (!file.good()){
		std::cout << "Can't read file " << filename << std::endl;
		std::terminate();
	}

	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&shaderCode[0], shaderCode.size());
	file.close();
	return shaderCode;
}

GLuint ShaderLoader::CreateShader(GLenum shaderType, std::string
	source, char* shaderName)
{

	int compile_result = 0;

	GLuint shader = glCreateShader(shaderType);
	const char *shader_code_ptr = source.c_str();
	const int shader_code_size = source.size();

	glShaderSource(shader, 1, &shader_code_ptr, &shader_code_size);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);

	//check for errors
	if (compile_result == GL_FALSE)
	{

		int info_log_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector<char> shader_log(info_log_length);
		glGetShaderInfoLog(shader, info_log_length, NULL, &shader_log[0]);
		std::cout << "ERROR compiling shader: " << shaderName << std::endl << &shader_log[0] << std::endl;
		return 0;
	}
	return shader;
}

GLuint ShaderLoader::CreateProgram(char* vertexShaderFilename,
	char* fragmentShaderFilename)
{
	std::string vertexShaderName = vertexShaderFilename;
	std::string fragmentShaderName = fragmentShaderFilename;

	if (Programs.find(vertexShaderName + fragmentShaderName) != Programs.end())
	{
		return Programs.at(vertexShaderName + fragmentShaderName);
	}

	//read the shader files and save the code
	std::string vertex_shader_code = ReadShader(vertexShaderFilename);
	std::string fragment_shader_code = ReadShader(fragmentShaderFilename);

	GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_code, "vertex shader");
	GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_code, "fragment shader");

	VertexShaders.insert(std::pair<char*, GLuint>(vertexShaderFilename, vertex_shader));
	FragmentShaders.insert(std::pair<char*, GLuint>(fragmentShaderFilename, fragment_shader));

	int link_result = 0;
	//create the program handle, attatch the shaders and link it
	GLuint program = glCreateProgram();

	// Put the program into map
	Programs.insert(std::pair<std::string, GLuint>(vertexShaderName + fragmentShaderName, program));

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	//check for link errors
	if (link_result == GL_FALSE)
	{

		int info_log_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector<char> program_log(info_log_length);
		glGetProgramInfoLog(program, info_log_length, NULL, &program_log[0]);
		std::cout << "Shader Loader : LINK ERROR" << std::endl << &program_log[0] << std::endl;
		return 0;
	}
	return program;
}

GLuint ShaderLoader::CreateProgram(char * VertexShaderFileName, char * FragmentShaderFilename, char * GeometryShaderFilename)
{
	std::string vertexShaderName = VertexShaderFileName;
	std::string fragmentShaderName = FragmentShaderFilename;
	std::string geometryShaderName = GeometryShaderFilename;

	if (Programs.find(vertexShaderName + fragmentShaderName + geometryShaderName) != Programs.end())
	{
		return Programs.at(vertexShaderName + fragmentShaderName + geometryShaderName);
	}

	//read the shader files and save the code
	std::string vertex_shader_code = ReadShader(VertexShaderFileName);
	std::string fragment_shader_code = ReadShader(FragmentShaderFilename);
	std::string geometry_shader_code = ReadShader(GeometryShaderFilename);

	GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_code, "vertex shader");
	GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_code, "fragment shader");
	GLuint geometry_shader = CreateShader(GL_GEOMETRY_SHADER, geometry_shader_code, "geometry shader");

	VertexShaders.insert(std::pair<char*, GLuint>(VertexShaderFileName, vertex_shader));
	FragmentShaders.insert(std::pair<char*, GLuint>(FragmentShaderFilename, fragment_shader));
	GeometryShaders.insert(std::pair<char*, GLuint>(GeometryShaderFilename, geometry_shader));

	int link_result = 0;
	//create the program handle, attatch the shaders and link it
	GLuint program = glCreateProgram();

	// Put the program into map
	Programs.insert(std::pair<std::string, GLuint>(vertexShaderName + fragmentShaderName + geometryShaderName, program));

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glAttachShader(program, geometry_shader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	//check for link errors
	if (link_result == GL_FALSE)
	{

		int info_log_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector<char> program_log(info_log_length);
		glGetProgramInfoLog(program, info_log_length, NULL, &program_log[0]);
		std::cout << "Shader Loader : LINK ERROR" << std::endl << &program_log[0] << std::endl;
		return 0;
	}
	return program;
}

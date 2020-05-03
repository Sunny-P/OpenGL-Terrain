#pragma once
#include <string>
#include <vector>
#include <random>

#include "Dependencies\glew\glew.h"
#include "Dependencies\glm\glm.hpp"
//#include "Dependencies\glm\gtc\half_float.hpp"
#include "Dependencies\glm\gtc\quaternion.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"

class Camera;

struct TerrainData {
	float height;
	float row;
	float col;
};

struct TerrainRectangle {
	TerrainData topLeft;
	TerrainData topRight;
	TerrainData bottomLeft;
	TerrainData bottomRight;
};

class Terrain
{
public:
	Terrain(GLuint _tex, GLuint _blendedTex1 = NULL, GLuint _blendedTex2 = NULL, GLuint _highBlendedTex = NULL, int rows = 513, int cols = 513);
	Terrain(GLuint _tex, std::string terrainDataFilepath);
	~Terrain();

	float GetWidth() const;
	float GetDepth() const;
	float GetHeight(float x, float z) const;

	void Init();
	void Render(GLuint program);
	void Update();

	bool InBounds(int i, int j);
private:
	void LoadHeightMap();
	void GenerateTerrain();
	
	float Average(int i, int j);
	void BuildVertices();

	void Smooth();

	void GenerateAdditionalBuffers();
	float GetPercentage(float value, const float min, const float max);

	//float RandomFloatInRange(float lower, float upper);	// Moved to Utility.h - Generic function to return RandomFlInRange is useful more than just here
	int RandomIntInRange(int lower, int upper);	// TODO: Move to Utility.h and seed using mt19937 once but still have access to the numberEngine

	// Smooths a point by averaging using its neighbours
	float SmoothPoint(int x, int y);
	float RandomNoise(int x, int y);
	float LinearInterpolation(float a, float b, float x);
	float CosineInterpolation(float a, float b, float x);
	float CubicInterpolation(float v0, float v1, float v2, float v3, float x);
	
	float Noise(float x, float y);
	float TotalNoisePerPoint(int x, int y);

	int noiseNumber;
	int smallerRandom, middleRandom, largeRandom;

	float heightScale;
	float heightOffset;
	int numRows;
	int numCols;
	float cellSpacing;

	int numVertices;
	int numFaces;

	std::vector<float> heightMap;
	std::vector<glm::vec4> colourBuffer;

	GLuint tex;
	GLuint blendedTexture1;
	GLuint blendedTexture2;
	GLuint highBlendedTex;
	GLuint VAO, VBO, EBO;
	GLuint colourVBO;

	int indexCount;
	int drawType;

	glm::vec3 position;
	glm::vec3 zRotation;
	glm::vec3 scale;

	float rotationAngle;

	glm::mat4 modelMatrix;
	void UpdateModelMatrix();

	float shininess;

	float texCScaling;

	bool loadFromFile;
	std::string terrainDataFilepath;
};
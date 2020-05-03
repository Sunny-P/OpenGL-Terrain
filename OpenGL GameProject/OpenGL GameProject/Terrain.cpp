#include "Terrain.h"
#include "Camera.h"
#include "Utility.h"

#include <fstream>
#include <sstream>

struct TerrainVertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texC;
};

Terrain::Terrain(GLuint _tex, GLuint _blendedTex1, GLuint _blendedTex2, GLuint _highBlendedTex, int rows, int cols)
{
	heightScale = 100.0f;
	heightOffset = 40.0f;
	numRows = rows;
	numCols = cols;
	cellSpacing = 5.0f;

	tex = _tex;
	if (_blendedTex1 == NULL)
	{
		blendedTexture1 = tex;
	}
	else
	{
		blendedTexture1 = _blendedTex1;
	}
	if (_blendedTex2 == NULL)
	{
		blendedTexture2 = tex;
	}
	else
	{
		blendedTexture2 = _blendedTex2;
	}
	if (_highBlendedTex == NULL)
	{
		highBlendedTex = tex;
	}
	else
	{
		highBlendedTex = _highBlendedTex;
	}

	position = glm::vec3(0.0f, 0.0f, 0.0f);
	zRotation = glm::vec3(0.0f, 0.0f, 1.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	rotationAngle = 0.0f;

	shininess = 5.0f;

	texCScaling = 256.0f;

	loadFromFile = false;
}

Terrain::Terrain(GLuint _tex, std::string terrainDataFilepath)
{
	heightScale = 1;
	heightOffset = 0;
	numRows = 513;
	numCols = 513;
	cellSpacing = 5;

	tex = _tex;

	position = glm::vec3(0.0f, 0.0f, 0.0f);
	zRotation = glm::vec3(0.0f, 0.0f, 1.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	rotationAngle = 0.0f;

	shininess = 1.0f;

	texCScaling = 256.0f;

	loadFromFile = true;
	this->terrainDataFilepath = terrainDataFilepath;
}

Terrain::~Terrain()
{

}

float Terrain::GetWidth() const
{
	return (numCols - 1)*cellSpacing;
}

float Terrain::GetDepth() const
{
	return (numRows - 1)*cellSpacing;
}

float Terrain::GetHeight(float x, float z) const
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f*GetWidth()) / cellSpacing;
	float d = (z - 0.5f*GetDepth()) / -cellSpacing;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = heightMap[row*numCols + col];
	float B = heightMap[row*numCols + col + 1];
	float C = heightMap[(row + 1)*numCols + col];
	float D = heightMap[(row + 1)*numCols + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
}

void Terrain::Init()
{
	numVertices = numRows*numCols;
	numFaces = (numRows - 1)*(numCols - 1) * 2;

	std::cout << "Terrain Generation started." << std::endl;
	if (loadFromFile)
	{
		LoadHeightMap();
	}
	else
	{
		// Generate terrain point data procedurally
		GenerateTerrain();
	}
	std::cout << "Terrain Generation complete!" << std::endl;

	std::cout << "Smoothing Terrain started." << std::endl;
	Smooth();
	std::cout << "Smoothing Terrain complete!" << std::endl;

	std::cout << "Building Terrain Vertices started." << std::endl;
	BuildVertices();
	std::cout << "Building Terrain Vertices complete!" << std::endl;

	UpdateModelMatrix();
}

void Terrain::Render(GLuint program)
{
	glUseProgram(program);

	GLuint modelLoc = glGetUniformLocation(program, "modelMat");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniform1f(glGetUniformLocation(program, "shininess"), shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blendedTexture1);
	glUniform1i(glGetUniformLocation(program, "blendedTex1"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, blendedTexture2);
	glUniform1i(glGetUniformLocation(program, "blendedTex2"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, highBlendedTex);
	glUniform1i(glGetUniformLocation(program, "highBlendedTex"), 3);

	glBindVertexArray(VAO);
	glDrawElements(drawType, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Terrain::Update()
{
	UpdateModelMatrix();
}

void Terrain::LoadHeightMap()
{
	// A height for each vertex
	std::vector<unsigned char> in(numRows * numCols);

	// Open the file.
	std::ifstream inFile;
	inFile.open(terrainDataFilepath, std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	// Copy the array data into a float array, and scale and offset the heights.
	heightMap.resize(numRows * numCols, 0);
	for (int i = 0; i < numRows * numCols; ++i)
	{
		heightMap[i] = (float)in[i] * heightScale + heightOffset;
	}
}

void Terrain::GenerateTerrain()
{
	heightMap.resize(numRows * numCols);

	// Basic Random
	/*float originalLower, originalUpper, lower, upper;
	originalLower = 0.0f;
	originalUpper = 20.0f;
	lower = originalLower;
	upper = originalUpper;

	for (int i = 0; i < numRows * numCols; ++i)
	{
		float randHeight = RandomFloatInRange(lower, upper) * heightScale + heightOffset;
		heightMap[i] = randHeight;
	}
	*/

	// Perlin Noise

	// noiseNumber is essentially the seed for the terrain, the range is the number of seeds that can be generated that are unique from eachother
	noiseNumber = RandomIntInRange(-RAND_MAX, RAND_MAX);
	//smallerRandom = RandomIntInRange(1, 249999);
	smallerRandom = RandomIntInRange(1, 2000000000);
	//middleRandom = RandomIntInRange(250000, 1250000);
	middleRandom = RandomIntInRange(250000, 2000000000);
	largeRandom = RandomIntInRange(10000000, 2000000000);
	
	for (int i = 0; i < numRows * numCols; ++i)
	{
		int x, y;
		x = i % numCols;
		y = i / numRows;

		heightMap[i] = TotalNoisePerPoint(x, y) * heightScale + heightOffset;
	
		if (i % 10000 == 0)
		{
			std::cout << "Terrain generation: " << ((float)i / ((float)numRows * (float)numCols)) * 100 << "%" << std::endl;
		}
	}
	
	std::cout << "Combined Seed: " << noiseNumber << "+" << smallerRandom << "+" << middleRandom << "+" << largeRandom << std::endl;

}

bool Terrain::InBounds(int i, int j)
{
	// True if ij are valid indices; false otherwise.
	return
		i >= 0 && i < numRows &&
		j >= 0 && j < numCols;
}

float Terrain::Average(int i, int j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	for (int m = i - 1; m <= i + 1; ++m)
	{
		for (int n = j - 1; n <= j + 1; ++n)
		{
			if (InBounds(m, n))
			{
				avg += heightMap[m * numCols + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

void Terrain::BuildVertices()
{
	std::vector<TerrainVertex> terrainVertices(numVertices);

	float halfWidth = (numCols - 1) * cellSpacing * 0.5f;
	float halfDepth = (numRows - 1) * cellSpacing * 0.5f;

	float du = 1.0f / (numCols - 1);
	float dv = 1.0f / (numRows - 1);
	for (int i = 0; i < numRows; ++i)
	{
		float z = halfDepth - i * cellSpacing;
		for (int j = 0; j < numCols; ++j)
		{
			float x = -halfWidth + j * cellSpacing;

			float y = heightMap[i * numCols + j];
			terrainVertices[i * numCols + j].pos = glm::vec3(x, y, z);
			terrainVertices[i * numCols + j].normal = glm::vec3(0.0f, 1.0f, 0.0f);

			// Stretch texture over grid.
			terrainVertices[i * numCols + j].texC.x = j * du;
			terrainVertices[i * numCols + j].texC.y = i * dv;
			
		}
	}
	std::cout << "Building Terrain Vertices: 24.6%" << std::endl;

	// Estimate normals for interior nodes using central difference.
	float invTwoDX = 1.0f / (2.0f*cellSpacing);
	float invTwoDZ = 1.0f / (2.0f*cellSpacing);
	for (int i = 2; i < numRows - 1; ++i)
	{
		for (int j = 2; j < numCols - 1; ++j)
		{
			float t = heightMap[(i - 1)*numCols + j];
			float b = heightMap[(i + 1)*numCols + j];
			float l = heightMap[i*numCols + j - 1];
			float r = heightMap[i*numCols + j + 1];

			glm::vec3 tanZ(0.0f, (t - b)*invTwoDZ, 1.0f);
			glm::vec3 tanX(1.0f, (r - l)*invTwoDX, 0.0f);

			glm::vec3 N = glm::cross(tanZ, tanX);
			N = glm::normalize(N);

			terrainVertices[i*numCols + j].normal = N;
		}
	}
	std::cout << "Building Terrain Vertices: 52.124%" << std::endl;

	// Convert DirectX format to open GL
	std::vector<GLfloat> openGLVertices(numVertices * 9);

	int vertex = 0;

	for (int i = 0; i < numRows; i++)
	{
		float z = halfDepth - i * cellSpacing;
		for (int j = 0; j < numCols; j++)
		{
			float x = -halfWidth + j * cellSpacing;
			float y = heightMap[i * numCols + j];

			// Positions
			openGLVertices[vertex + 0] = terrainVertices[i * numCols + j].pos.x;
			openGLVertices[vertex + 1] = terrainVertices[i * numCols + j].pos.y;
			openGLVertices[vertex + 2] = terrainVertices[i * numCols + j].pos.z;

			// Normals
			openGLVertices[vertex + 3] = terrainVertices[i * numCols + j].normal.x;
			openGLVertices[vertex + 4] = terrainVertices[i * numCols + j].normal.y;
			openGLVertices[vertex + 5] = terrainVertices[i * numCols + j].normal.z;

			// Texture coordinates
			openGLVertices[vertex + 6] = terrainVertices[i * numCols + j].texC.x * texCScaling;
			openGLVertices[vertex + 7] = terrainVertices[i * numCols + j].texC.y * texCScaling;

			// Terrain Height Value
			openGLVertices[vertex + 8] = y + RandomFloatInRange(-1.0f, 1.0f);

			vertex += 9;
		}
	}
	std::cout << "Building Terrain Vertices: 75.783%" << std::endl;

	std::vector<GLuint> terrainIndices(numFaces * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	int k = 0;
	for (int i = 0; i < numRows - 1; ++i)
	{
		for (int j = 0; j < numCols - 1; ++j)
		{
			terrainIndices[k] = i * numCols + j;
			terrainIndices[k + 1] = i * numCols + j + 1;
			terrainIndices[k + 2] = (i + 1)*numCols + j;

			terrainIndices[k + 3] = (i + 1)*numCols + j;
			terrainIndices[k + 4] = i * numCols + j + 1;
			terrainIndices[k + 5] = (i + 1)*numCols + j + 1;

			k += 6; // next quad
		}
	}
	std::cout << "Building Terrain Vertices: 99.386%" << std::endl;

	//colourBuffer.resize(numRows * numCols);

	//for (unsigned int j = 0; j < numRows; ++j)
	//{
	//	for (unsigned i = 0; i < numCols; ++i)
	//	{
	//		unsigned int index = (j * numCols) + i;
	//		float heightValue = GetHeight(j, i);

	//		// Blend 3 textures depending on the height of the terrain
	//		float tex0Contribution = 1.0f - GetPercentage(heightValue, 0.0f, 0.75f);
	//		float tex2Contribution = 1.0f - GetPercentage(heightValue, 0.75f, 1.0f);

	//		colourBuffer[index] = glm::vec4(tex0Contribution, tex0Contribution, tex0Contribution, tex2Contribution);
	//		
	//		//float fTexture0Contribution = glm::saturate(glm::dot(terrainVertices[index].normal, glm::vec3(0.0f, 1.0f, 0.0f)) - 0.1f);
	//		float fTexture0Contribution = glm::clamp(glm::dot(terrainVertices[index].normal, glm::vec3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f);
	//		colourBuffer[index] = glm::vec4(fTexture0Contribution, fTexture0Contribution, fTexture0Contribution, colourBuffer[index].w);
	//	}
	//}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, numVertices * 9 * sizeof(openGLVertices[0]), &openGLVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces * 3 * sizeof(GLfloat), &terrainIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	indexCount = numFaces * 3;
	drawType = GL_TRIANGLES;
}

void Terrain::Smooth()
{
	std::vector<float> dest(heightMap.size());

	for (int i = 0; i < numRows; ++i)
	{
		for (int j = 0; j < numCols; ++j)
		{
			dest[i*numCols + j] = Average(i, j);
		}
	}

	// Replace the old heightmap with the filtered one.
	heightMap = dest;
}

void Terrain::GenerateAdditionalBuffers()
{
	
}

float Terrain::GetPercentage(float value, const float min, const float max)
{
	value = glm::clamp(value, min, max);
	return (value - min) / (max - min);
}

//float Terrain::RandomFloatInRange(float lower, float upper)
//{
//	return lower + static_cast<float>(rand() / (static_cast<float>(RAND_MAX/(upper - lower))));
//}

int Terrain::RandomIntInRange(int lower, int upper)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> intDistribution(lower, upper);

	return intDistribution(gen);
	//return lower + (rand() / (RAND_MAX / (upper - lower)));
}

float Terrain::RandomNoise(int x, int y)
{
	//int n = x + y * 57;
	int n = x + y * noiseNumber;
	n = (n << 13) ^ n; 

	//int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff; 
	int t = (n * (n * n * smallerRandom + middleRandom) + largeRandom) & 0x7fffffff;

	return 1.0 - double(t) * 0.931322574615478515625e-9;
}

float Terrain::LinearInterpolation(float a, float b, float x)
{
	return a * (1 - x) + b * x;
}

float Terrain::CosineInterpolation(float a, float b, float x)
{
	float ft, f;
	ft = x * 3.1415927f; 
	f = (1.0f - cos(ft)) * 0.5f;
	return a * (1.0f - f) + b * f;
}

float Terrain::CubicInterpolation(float v0, float v1, float v2, float v3, float x)
{
	float P, Q, R, S;
	P = (v3 - v2) - (v0 - v1);
	Q = (v0 - v1) - P;
	R = v2 - v0;
	S = v1;
	
	return (P * std::powf(x, 3.0f)) + (Q * std::powf(x, 2.0f)) + (R * x) + S;
}

float Terrain::Noise(float x, float y)
{
	float fractionalX = x - int(x);
	float fractionalY = y - int(y);

	// Smooths
	float v1 = SmoothPoint(int(x), int(y));
	float v2 = SmoothPoint(int(x) + 1, int(y));
	float v3 = SmoothPoint(int(x), int(y) + 1);
	float v4 = SmoothPoint(int(x) + 1, int(y) + 1);

	// Interpolates

	float i1 = LinearInterpolation(v1, v2, fractionalX);
	float i2 = LinearInterpolation(v3, v4, fractionalX);

	return LinearInterpolation(i1, i2, fractionalY);
}

float Terrain::TotalNoisePerPoint(int x, int y)
{
	int octaves = 8; 
	float zoom = 20.0f; 
	float persistence = 0.5f; 
	float total = 0.0f;

	for (int i = 0; i < octaves - 1; i++) 
	{
		float frequency = pow(2, i) / zoom; 
		float amplitude = pow(persistence, i);
		total += Noise((float)x * frequency, (float)y * frequency) *  amplitude;
	} 
	return total;
}

float Terrain::SmoothPoint(int x, int y)
{
	float corners, sides, center;

	corners = (RandomNoise(x - 1, y - 1) + RandomNoise(x + 1, y - 1) + RandomNoise(x - 1, y + 1) + RandomNoise(x + 1, y + 1)) / 16;

	sides = (RandomNoise(x - 1, y) + RandomNoise(x + 1, y) + RandomNoise(x, y - 1) + RandomNoise(x, y + 1)) / 8;
	center = RandomNoise(x, y) / 4;

	return corners + sides + center;
}

void Terrain::UpdateModelMatrix()
{
	modelMatrix = glm::translate(glm::mat4(), position)
		* glm::rotate(glm::mat4(), glm::radians(rotationAngle), zRotation)
		* glm::scale(glm::mat4(), scale);
}

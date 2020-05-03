#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in float terrainHeight;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;
out float fragTerrainHeight;

uniform float deltaTime;

uniform mat4 PVMatrix;
uniform mat4 modelMat;

void main(void)
{
	gl_Position = PVMatrix * modelMat * vec4(position, 1.0);
	fragPos = vec3(modelMat * vec4(position, 1.0f));
	fragNormal = mat3(transpose(inverse(modelMat))) * normal;
	fragTexCoord = texCoord;
	fragTerrainHeight = terrainHeight;
}
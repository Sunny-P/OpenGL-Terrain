#version 450 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;
in float fragTerrainHeight;

out vec4 color;

uniform float deltaTime;
uniform sampler2D tex;
uniform sampler2D blendedTex1;
uniform sampler2D blendedTex2;
uniform sampler2D highBlendedTex;

uniform vec3 ambient;
uniform vec3 lightColour;
uniform vec3 lightPos;

uniform vec3 camPos;
uniform float lightSpecStr;
uniform float shininess;

float GetPercentage(float value, const float min, const float max)
{
	value = clamp(value, min, max);
	return (value - min) / (max - min);
}

float Lerpf(float x, float y, float f)
{
	return (x * (1.0f - f)) + (y * f);
}

void main(void)
{
	// THE TERRAIN DOES NOT UTILISE LIGHTING AS OF NOW
	// UNCOMMENT IF YOU NEED LIGHTING ON TERRAIN
	//========================================================================================
	// Light Direction
	//vec3 norm = normalize(fragNormal);
	//vec3 lightDir = normalize(fragPos - lightPos);

	// Diffuse colouring
	//float diffuseStr = max(dot(norm, -lightDir), 0.0f);
	//vec3 diffuse = diffuseStr * lightColour;

	// Specular highlight
	//vec3 negViewDir = normalize(camPos - fragPos);
	//vec3 reflectDir = reflect(lightDir, norm);
	//float spec = pow(max(dot(negViewDir, reflectDir), 0.0f), shininess);
	//vec3 halfwayVec = normalize(-lightDir + negViewDir);
	//float spec = pow(max(dot(norm, halfwayVec), 0.0f), shininess);
	//vec3 specular = lightSpecStr * spec * lightColour;
	//========================================================================================

	// The amount of priority each texture gets
	float texAlphaStrength = 0.8f;
	float blended1AlphaStrength = 0.0f;
	float blended2AlphaStrength = 0.1f;

	//float tex0Contribution = 1.0f - GetPercentage(fragTerrainHeight, 0.0f, 0.45f);
	//float tex2Contribution = 1.0f - GetPercentage(fragTerrainHeight, 0.75f, 1.0f);
	//float tex1Contribution = tex0Contribution - tex2Contribution;

	//vec4 terrainTexel = vec4(tex0Contribution, tex0Contribution, tex0Contribution, tex2Contribution);

	// This returns a value based on the angle the faces normal is on. The '- 0.05f' reduces the required angle to start applying Stone texture.
	// Reduce the value to have more stone show up on less angled surfaces, Increase the value to have less stone show up - Only on more angled surfaces
	float fTexture0Contribution = clamp(dot(fragNormal, vec3(0.0f, 1.0f, 0.0f)) - 0.05f, 0.0f, 1.0f);

	// Normal single texturing with lighting
	// vec4((ambient + diffuse + specular), 1.0f)

	// The first 3 components of the Vec4 are the colour components.
	// Based on the Surface Normal, How much contribution does the grass and stone have. More vertical faces have more Stone contribution
	// 
	// The 4th component is the Alpha channel
	// Based on the fragTerrainHeight, how much contribution does the grass and snow have. Higher up, more contribution for Snow.
	//
	// Initially output the normal grass surface at full.
	//
	// Using the surface normal change the RGB components to be more stone texture on vertical surfaces
	//
	// Based on the fragTerrainHeight, add the snowy texture as necessary using the alpha channel

	// We have the colour and alpha components of all textures here
	vec4 grass = vec4(1.0f, 1.0f, 1.0f, 1.0f) * texture(tex, fragTexCoord);
	vec4 stone = vec4(1.0f, 1.0f, 1.0f, 1.0f) * texture(blendedTex1, fragTexCoord);
	vec4 sand = vec4(1.0f, 1.0f, 1.0f, 1.0f) * texture(blendedTex2, fragTexCoord);
	vec4 snow = vec4(1.0f, 1.0f, 1.0f, 1.0f) * texture(highBlendedTex, fragTexCoord);
	vec4 finalTexture = grass;

	float sandHeightThreshold = 1.0f;
	float snowHeightThreshold = 80.0f;

	//color = (vec4((ambient + diffuse + specular), 1.0f) * texture(tex, fragTexCoord));// + (vec4((ambient + diffuse + specular)/3.0f, blended1AlphaStrength) * texture(blendedTex1, fragTexCoord)) + (vec4(tex0Contribution, tex0Contribution, tex0Contribution, tex2Contribution) * texture(blendedTex2, fragTexCoord));

	//
    // Texture Stage 0
    //
    // Simply output texture0 for stage 0.
    //
	//finalTexture = grass; // Already being done above
	//
    // Texture Stage 1
    //
    // Perform a linear interpolation between the output of stage 0 
    // (i.e texture0) and texture1 and use the RGB portion of the vertex's 
    // color to mix the two. 
    //
	
	//
    // Texture Stage 2
    //
    // Perform a linear interpolation between the output of stage 1 
    // (i.e texture0 mixed with texture1) and texture2 and use the ALPHA 
    // portion of the vertex's color to mix the two. 
    //
	//finalTexture.w = Lerpf(finalTexture.w, snow.w, tex2Contribution);

	float sandAlphaStrength = clamp(1.0f - (fragTerrainHeight/sandHeightThreshold), 0.0f, 1.0f);

	finalTexture.x = Lerpf(finalTexture.x, sand.x, sandAlphaStrength);
	finalTexture.y = Lerpf(finalTexture.y, sand.y, sandAlphaStrength);
	finalTexture.z = Lerpf(finalTexture.z, sand.z, sandAlphaStrength);

	float highTexBlendStrength = clamp((1.0f - (fragTerrainHeight/snowHeightThreshold)) * -1.0f, 0.0f, 1.0f);
	finalTexture.x = Lerpf(finalTexture.x, snow.x, highTexBlendStrength);
	finalTexture.y = Lerpf(finalTexture.y, snow.y, highTexBlendStrength);
	finalTexture.z = Lerpf(finalTexture.z, snow.z, highTexBlendStrength);

	finalTexture.x = Lerpf(stone.x, finalTexture.x, fTexture0Contribution);
	finalTexture.y = Lerpf(stone.y, finalTexture.y, fTexture0Contribution);
	finalTexture.z = Lerpf(stone.z, finalTexture.z, fTexture0Contribution);
	//finalTexture = finalTexture + snow;

	color = finalTexture;
}
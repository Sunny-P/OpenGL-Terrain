#pragma once
#include "ShaderLoader.h"
#include "Dependencies\soil\SOIL.h"
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"

#include "Game.h"
#include "Camera.h"
#include "Player.h"
#include "TextLabel.h"
#include "CubeMesh.h"
#include "Mesh_Sphere.h"
#include "texture.h"
#include "CubeMap.h"
#include "Quad.h"
#include "Model.h"
#include "Enemy.h"
#include "Audio.h"
#include "WorldObject.h"
#include "Input.h"
#include "BasicShapes2DPhysics.h"
#include "Terrain.h"
#include "GeometryModel.h"
#include "ParticleSystem.h"

Game::Game(int _width, int _height)
{
	/*playerTextureFilePath = "Resources/Textures/Player_Ship.png";
	backgroundFilePath = "Resources/Textures/water.png";*/
	currentGameState = DEFAULT;

	//audioSys = new Audio();

	width = _width;
	height = _height;

	// Default values for light
	ambientStr = 0.3f;
	ambientColour = glm::vec3(0.75f, 0.75f, 0.75f);
	ambient = ambientStr * ambientColour;

	//lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
	lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
	lightPos = glm::vec3(-500.0f, 500.0f, 500.0f);

	specularStr = 1.0f;

	// Structure with light information to pass into models as uniforms for model shader
	// so lighting can work on models
	lighting.ambient = ambient;
	lighting.ambientColour = ambientColour;
	lighting.ambientStr = ambientStr;
	lighting.lightColour = lightColour;
	lighting.lightPos = lightPos;
	lighting.specularStr = specularStr;

	PopulateCubemapFilePathVec();

	textureDetails tempDetails;
	tempDetails = CreateTexture("Resources/Textures/Objects/crate3.jpg");
	cube = new CubeMesh(tempDetails.tex);
	secondCube = new CubeMesh(tempDetails.tex);

	GLuint terrainTex0, terrainTex1, terrainTex2, terrainTex3;
	tempDetails = CreateTexture("Resources/Textures/Materials/highdef_grass.jpg", GL_MIRRORED_REPEAT);
	terrainTex0 = tempDetails.tex;

	tempDetails = CreateTexture("Resources/Textures/Materials/rock.png", GL_MIRRORED_REPEAT);
	terrainTex1 = tempDetails.tex;

	tempDetails = CreateTexture("Resources/Textures/Materials/sand.jpg", GL_MIRRORED_REPEAT);
	terrainTex2 = tempDetails.tex;

	tempDetails = CreateTexture("Resources/Textures/Materials/snow.jpg", GL_MIRRORED_REPEAT);
	terrainTex3 = tempDetails.tex;

	terrain = new Terrain(terrainTex0, terrainTex1, terrainTex2, terrainTex3);
	//terrain = new Terrain(tempDetails.tex);

	camera = new Camera(PERSP);

	// Change lightColour so enemies can be shaded differently
	//lighting.lightColour = glm::vec3(0.0f, 0.502f, 0.0f);

	skybox = new CubeMap(camera, vecCubeMapFilePaths);

	outlineObjects = false;
	wireframeOn = false;
	scissorTestOn = false;

	mainPlayer = new Player(cube);
	
	camera->SetCamPos(glm::vec3(0.0f, 10.0f, -3.0f));
	//keyDelayWaitTime = 0.1f;

	geometryModel = new GeometryModel(camera);
	geometryModel->SetPosition(glm::vec3(0.0f, 100.0f, 0.0f));

	gravity = 9.82f;
	playerJumpSpeed = 0.0f;
	playerJumpHeight = 10.0f;

	playerMoveSpeed = 8.0f;

	
}

Game::~Game()
{
	//delete audioSys;
	delete cube;
	delete terrain;
	delete geometryModel;

	for (auto it = textureMap.begin(); it != textureMap.end(); ++it)
	{
		delete it->second.texture;
	}
	textureMap.clear();

	for (auto it = modelMap.begin(); it != modelMap.end(); ++it)
	{
		delete it->second;
	}
	modelMap.clear();

	delete camera;
	delete mainPlayer;

	delete fountainParticleSys;
}

void Game::init()
{
	previousTimeStamp = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	ShaderLoader shaderLoader;
	program = shaderLoader.CreateProgram("Resources/Shaders/VertexShader.vs", "Resources/Shaders/FragmentShader.fs");
	reflectPhongProgram = shaderLoader.CreateProgram("Resources/Shaders/BlinnPhong+Reflect-VertexShader.vs", "Resources/Shaders/BlinnPhong+Reflect-FragmentShader.fs");
	phongProgram = shaderLoader.CreateProgram("Resources/Shaders/BlinnPhong-VertexShader.vs", "Resources/Shaders/BlinnPhong-FragmentShader.fs");
	cubemapProgram = shaderLoader.CreateProgram("Resources/Shaders/CubeMap/CubeMapVS.vs", "Resources/Shaders/CubeMap/CubeMapFS.fs");
	outlineProgram = shaderLoader.CreateProgram("Resources/Shaders/VertexShader.vs", "Resources/Shaders/OutlineShader.fs");
	fogProgram = shaderLoader.CreateProgram("Resources/Shaders/FogVertexShader.vs", "Resources/Shaders/FogFragmentShader.fs");
	geometryProgram = shaderLoader.CreateProgram("Resources/Shaders/GeometryVertexShader.vs", "Resources/Shaders/GeometryFragmentShader.fs", "Resources/Shaders/GeometryShader.gs");
	particleProgram = shaderLoader.CreateProgram("Resources/Shaders/ParticleVertexShader.vs", "Resources/Shaders/ParticleFragmentShader.fs", "Resources/Shaders/ParticleGeometryShader.gs");
	multiTexturedProgram = shaderLoader.CreateProgram("Resources/Shaders/TerrainMultiTexBlinnPhong-VertexShader.vs", "Resources/Shaders/TerrainMultiTexBlinnPhong-FragmentShader.fs");

	cube->init();
	secondCube->init();
	mainPlayer->init();

	camera->SetFollowedPlayer(mainPlayer);

	CreateTextLabels();

	//-- Default values for Physics ---------------------------
	//gravity = b2Vec2(0.0f, -9.8f);
	//physicsWorld = new b2World(gravity);

	//velocityIterations = 8;
	//positionIterations = 3;
	//---------------------------------------------------------

	//textureDetails tempDetails;
	//tempDetails = CreateTexture("Resources/Textures/Objects/crate3.jpg");

	//ground = new BasicShapes2DPhysics(tempDetails.tex, physicsWorld, SQUARE, true);
	//ground->SetScale((float)SCR_WIDTH, 20.0f, 1.0f);
	//ground->SetPosition((float)CENTRE_X, 3.0f, 0.0f);

	//leftBoundWall = new BasicShapes2DPhysics(tempDetails.tex, physicsWorld, SQUARE, true);
	//leftBoundWall->SetScale(20.0f, 650.0f, 1.0f);
	//leftBoundWall->SetPosition(0.0f, (float)CENTRE_Y, 0.0f);

	terrain->Init();

	textureDetails tempDetails;
	tempDetails = CreateTexture("Resources/Textures/waterball.png");

	float terrainHeight = terrain->GetHeight(0.0f, 0.0f) + (cube->GetHeight() * 0.5f);
	fountainParticleSys = new ParticleSystem(glm::vec3(0.0f, terrainHeight, 0.0f), camera, tempDetails.tex);
	fountainParticleSys->Init();
}

void Game::render()
{
	// Render Skybox no matter what
	skybox->Render(cubemapProgram);

	switch (currentGameState)
	{
	case DEFAULT:
		terrain->Render(multiTexturedProgram);

		cube->render(phongProgram);
		secondCube->render(phongProgram);

		geometryModel->Render(geometryProgram);
		fountainParticleSys->Render(particleProgram);

		RenderGameScene();
		break;

	default:
		break;
	}

	RenderGameText();
}

void Game::update()
{
	//audioSys->Update();
	// Calc deltaTime
	currentTime = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (currentTime - previousTimeStamp) * 0.001f;
	previousTimeStamp = currentTime;

	//physicsDeltaT = deltaTime * 2.0f;

	glUseProgram(phongProgram);
	GLuint currentTimeL = glGetUniformLocation(phongProgram, "deltaTime");
	glUniform1f(currentTimeL, deltaTime);

	glUseProgram(multiTexturedProgram);
	glUniform1f(glGetUniformLocation(multiTexturedProgram, "deltaTime"), deltaTime);

	glUseProgram(program);
	glUniform1f(glGetUniformLocation(program, "deltaTime"), deltaTime);

	glUseProgram(phongProgram);
	// Light uniforms
	glUniform3fv(glGetUniformLocation(phongProgram, "ambient"), 1, glm::value_ptr(ambient));
	glUniform3fv(glGetUniformLocation(phongProgram, "lightColour"), 1, glm::value_ptr(lightColour));
	glUniform3fv(glGetUniformLocation(phongProgram, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform1f(glGetUniformLocation(phongProgram, "lightSpecStr"), specularStr);

	glUseProgram(multiTexturedProgram);
	glUniform3fv(glGetUniformLocation(multiTexturedProgram, "ambient"), 1, glm::value_ptr(ambient));
	glUniform3fv(glGetUniformLocation(multiTexturedProgram, "lightColour"), 1, glm::value_ptr(lightColour));
	glUniform3fv(glGetUniformLocation(multiTexturedProgram, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform1f(glGetUniformLocation(multiTexturedProgram, "lightSpecStr"), specularStr);

	// Always update camera and skybox
	skybox->Update();

	

	switch (currentGameState)
	{
	case DEFAULT:
		//physicsWorld->Step((float32)physicsDeltaT, (int32)velocityIterations, (int32)positionIterations);
		terrain->Update();

		// Is the camera clipping through the terrain?
		if (camera->GetCamPos().y < terrain->GetHeight(camera->GetCamPos().x, camera->GetCamPos().z) + (camera->GetFOVY() * 0.02f))
		{
			camera->SetCameraSpringIsActive(true);
			camera->SetCamPos(glm::vec3(camera->GetCamPos().x, 
				terrain->GetHeight(camera->GetCamPos().x, camera->GetCamPos().z) + (camera->GetFOVY() * 0.02f),
				camera->GetCamPos().z));
		}
		else
		{
			camera->SetCameraSpringIsActive(false);
		}

		cube->update();
		secondCube->update();

		playerJumpSpeed -= gravity * deltaTime;

		// Is the player possessed object under the terrain? Change that if so.
		if (mainPlayer->GetPosition().y < (terrain->GetHeight(mainPlayer->GetPosition().x, mainPlayer->GetPosition().z) + (cube->GetHeight() * 0.5f)))
		{
			mainPlayer->SetPosition(mainPlayer->GetPosition().x,
				terrain->GetHeight(mainPlayer->GetPosition().x, mainPlayer->GetPosition().z) + (cube->GetHeight() * 0.5f),
				mainPlayer->GetPosition().z);
		}
		//cube->SetYPosition(terrain->GetHeight(cube->GetPosition().x, cube->GetPosition().z) + (cube->GetHeight() * 0.5f));

		// Move player vertically based on Jump input & gravity
		mainPlayer->IncrementPosition(0.0f, playerJumpSpeed * deltaTime, 0.0f);

		fountainParticleSys->Update(deltaTime);
		break;

	default:
		break;
	}

	glm::vec3 camPos = camera->GetCamPos();
	camera->update(deltaTime);

	glUseProgram(phongProgram);
	GLuint camPVMatLoc = glGetUniformLocation(phongProgram, "PVMatrix");
	glUniformMatrix4fv(camPVMatLoc, 1, GL_FALSE, glm::value_ptr(camera->GetPVMatrix()));
	glUniform3fv(glGetUniformLocation(phongProgram, "camPos"), 1, glm::value_ptr(camera->GetCamPos()));

	glUseProgram(multiTexturedProgram);
	glUniformMatrix4fv(glGetUniformLocation(multiTexturedProgram, "PVMatrix"), 1, GL_FALSE, glm::value_ptr(camera->GetPVMatrix()));
	glUniform3fv(glGetUniformLocation(multiTexturedProgram, "camPos"), 1, glm::value_ptr(camera->GetCamPos()));

	glUseProgram(outlineProgram);
	camPVMatLoc = glGetUniformLocation(outlineProgram, "PVMatrix");
	glUniformMatrix4fv(camPVMatLoc, 1, GL_FALSE, glm::value_ptr(camera->GetPVMatrix()));

	glUseProgram(fogProgram);
	camPVMatLoc = glGetUniformLocation(fogProgram, "PVMatrix");
	glUniformMatrix4fv(camPVMatLoc, 1, GL_FALSE, glm::value_ptr(camera->GetPVMatrix()));
	glUniform3fv(glGetUniformLocation(fogProgram, "camPos"), 1, glm::value_ptr(camera->GetCamPos()));

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "PVMatrix"), 1, GL_FALSE, glm::value_ptr(camera->GetPVMatrix()));

	glUseProgram(reflectPhongProgram);
	// Light uniforms
	glUniform3fv(glGetUniformLocation(reflectPhongProgram, "ambient"), 1, glm::value_ptr(ambient));
	glUniform3fv(glGetUniformLocation(reflectPhongProgram, "lightColour"), 1, glm::value_ptr(lightColour));
	glUniform3fv(glGetUniformLocation(reflectPhongProgram, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform1f(glGetUniformLocation(reflectPhongProgram, "lightSpecStr"), specularStr);

	glUniformMatrix4fv(glGetUniformLocation(reflectPhongProgram, "PVMatrix"), 1, GL_FALSE, glm::value_ptr(camera->GetPVMatrix()));
	glUniform3fv(glGetUniformLocation(reflectPhongProgram, "camPos"), 1, glm::value_ptr(camera->GetCamPos()));

	UpdateGameText();

	/*if (keyDelay > 0.0f)
	{
		keyDelay -= deltaTime;
	}
	else if (keyDelay < 0.0f)
	{
		keyDelay = 0.0f;
	}*/
}

void Game::RenderGameText()
{
	
}

void Game::RenderMainMenuText()
{
	
}

void Game::RenderGameScene()
{
	if (scissorTestOn)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor((GLint)(CENTRE_X-87.5f), (GLint)(CENTRE_Y-87.5f), 175, 175);
	}
	else if (!scissorTestOn)
	{
		glDisable(GL_SCISSOR_TEST);
	}

	if (wireframeOn)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
	}
	else if (!wireframeOn)
	{
		
		glPolygonMode(GL_FRONT, GL_FILL);
	}

	if (outlineObjects)
	{
		
	}
	else if (!outlineObjects)
	{
		
	}

	
}

void Game::UpdateGameText()
{
	
}

void Game::processInput()
{
	switch (currentGameState)
	{
	case DEFAULT:

		// Quit program using ESC
		if (Input::GetKeyState(INPUT_ESC) == DOWN_FIRST)
		{
			Shutdown();
		}

		// F key to turn wireframing on/off
		if ((Input::GetKeyState('f') == DOWN_FIRST) || (Input::GetKeyState('F') == DOWN_FIRST))
		{
			if (wireframeOn)
			{
				wireframeOn = false;
				//keyDelay = keyDelayWaitTime;
			}
			else if (!wireframeOn)
			{
				wireframeOn = true;
				//keyDelay = keyDelayWaitTime;
			}
		}

		// O key to turn outlining on/off
		if ((Input::GetKeyState('o') == DOWN_FIRST) || (Input::GetKeyState('O') == DOWN_FIRST))
		{
			if (outlineObjects)
			{
				outlineObjects = false;
				//keyDelay = keyDelayWaitTime;
			}
			else if (!outlineObjects)
			{
				outlineObjects = true;
				//keyDelay = keyDelayWaitTime;
			}
		}

		// T key to turn scissor testing on/off
		if ((Input::GetKeyState('t') == DOWN_FIRST) || (Input::GetKeyState('T') == DOWN_FIRST))
		{
			if (scissorTestOn)
			{
				scissorTestOn = false;
				//keyDelay = keyDelayWaitTime;
			}
			else if (!scissorTestOn)
			{
				scissorTestOn = true;
				//keyDelay = keyDelayWaitTime;
			}
		}

		// Camera movement
		
		
		newX = camera->GetCamPos().x - mainPlayer->GetPosition().x;
		newZ = camera->GetCamPos().z - mainPlayer->GetPosition().z;

		u = sqrt(pow(newX, 2) + pow(newZ, 2));
		newX = newX / u;
		newZ = newZ / u;

		if (Input::GetKeyState('w') == DOWN)
		{
			cubeMoveVec.x = newX * (-playerMoveSpeed * deltaTime);
			cubeMoveVec.z = newZ * (-playerMoveSpeed * deltaTime);
			//cube->IncrementPositionVec(glm::vec3(0.0f, 0.0f, 3.0f * deltaTime));

			//mainPlayer->IncrementPositionVec(cubeMoveVec);
			MovePlayerIfInTerrainBounds(cubeMoveVec);
		}
		if (Input::GetKeyState('s') == DOWN)
		{
			cubeMoveVec.x = newX * (playerMoveSpeed * deltaTime);
			cubeMoveVec.z = newZ * (playerMoveSpeed * deltaTime);
			//cube->IncrementPositionVec(glm::vec3(0.0f, 0.0f, -3.0f * deltaTime));
			//mainPlayer->IncrementPositionVec(cubeMoveVec);
			MovePlayerIfInTerrainBounds(cubeMoveVec);
		}
		if (Input::GetKeyState('a') == DOWN)
		{
			cubeMoveVec.x = -newZ * (playerMoveSpeed * deltaTime);
			cubeMoveVec.z = newX * (playerMoveSpeed * deltaTime);
			//cube->IncrementPositionVec(glm::vec3(3.0f * deltaTime, 0.0f, 0.0f));
			//mainPlayer->IncrementPositionVec(cubeMoveVec);
			MovePlayerIfInTerrainBounds(cubeMoveVec);
		}
		if (Input::GetKeyState('d') == DOWN)
		{
			cubeMoveVec.x = newZ * (playerMoveSpeed * deltaTime);
			cubeMoveVec.z = -newX * (playerMoveSpeed * deltaTime);
			//cube->IncrementPositionVec(glm::vec3(-3.0f * deltaTime, 0.0f, 0.0f));
			//mainPlayer->IncrementPositionVec(cubeMoveVec);
			MovePlayerIfInTerrainBounds(cubeMoveVec);
		}

		if (Input::GetKeyState('q') == DOWN)
		{
			mainPlayer->IncrementRotation(40.0f * deltaTime);
		}
		if (Input::GetKeyState('e') == DOWN)
		{
			mainPlayer->IncrementRotation(-40.0f * deltaTime);
		}

		// Is the player on the ground?
		if (mainPlayer->GetPosition().y < terrain->GetHeight(mainPlayer->GetPosition().x, mainPlayer->GetPosition().z) + (cube->GetHeight() * 0.5f))
		{
			playerJumpSpeed = 0.0f;
			// Spacebar press
			if (Input::GetKeyState(32) == DOWN_FIRST)
			{
				//std::cout << "Player jumped" << std::endl;
				playerJumpSpeed = playerJumpHeight;
			}
		}

		// R key to restart scene
		if ((Input::GetKeyState('r') == DOWN_FIRST) || (Input::GetKeyState('R') == DOWN_FIRST))
		{
			Restart();
			//keyDelay = keyDelayWaitTime;
		}

		if (Input::GetKeyState('k') == DOWN_FIRST)
		{
			mainPlayer->SetNewPossess(cube);
		}
		if (Input::GetKeyState('l') == DOWN_FIRST)
		{
			mainPlayer->SetNewPossess(secondCube);
		}
		//-------------------------------------------------------
		if (Input::GetMouseState(MOUSE_LEFT) == DOWN_FIRST)
		{
			camera->SetFOVY(camera->GetFOVY() - 1.0f);
			std::cout << "Current FOV: " << camera->GetFOVY() << std::endl;
		}
		if (Input::GetMouseState(MOUSE_LEFT) == DOWN)
		{
			
		}
		if (Input::GetMouseState(MOUSE_LEFT) == UP_FIRST)
		{
			
		}
		//------------------------------------------------------
		if (Input::GetMouseState(MOUSE_RIGHT) == DOWN_FIRST)
		{
			camera->SetFOVY(camera->GetFOVY() + 1.0f);
			std::cout << "Current FOV: " << camera->GetFOVY() << std::endl;
		}
		if (Input::GetMouseState(MOUSE_RIGHT) == DOWN)
		{

		}
		if (Input::GetMouseState(MOUSE_RIGHT) == UP_FIRST)
		{

		}
		
		break;
	default:
		break;
	}
}

void Game::keyboardDown(unsigned char key, int x, int y)
{
	Keystate[key] = DOWN;
}

void Game::keyboardUp(unsigned char key, int x, int y)
{
	Keystate[key] = UP;
}

void Game::mouseClick(int button, int state, int x, int y)
{
	if (button >= 3)
	{
		return;
	}
	//std::cout << "Clicked x: " << x << " | y: " << y << std::endl;
	MouseState[button] = (state == GLUT_DOWN) ? DOWN : UP;
	if (state == GLUT_DOWN && button == MOUSE_LEFT)
	{
		switch (currentGameState)
		{
		case DEFAULT:

			break;

		default:
			break;
		}
	}
}

void Game::mousePassiveMove(int _x, int _y)
{
	//std::cout << "Passive x: " << _x << " | y: " << _y << std::endl;
	//HoverMenuButton(_x, _y);
	//Input::MousePassiveMove(_x, _y);
	mouseX = _x;
	mouseY = _y;
}

void Game::mouseMove(int x, int y)
{
	//std::cout << "Clicked x: " << x << " | y: " << y << std::endl;
	mouseX = x;
	mouseY = y;
}

void Game::SpecialDown(int key, int x, int y)
{
	SpecialState[key] = DOWN;
}

void Game::SpecialUp(int key, int x, int y)
{
	SpecialState[key] = UP;
}

void Game::Shutdown()
{
	glutLeaveMainLoop();
}

void Game::HoverMenuButton(int _x, int _y)
{
	switch (currentGameState)
	{
	case DEFAULT:

		break;

	default:
		break;
	}
}

void Game::ClickMenuButton(int x, int y)
{
	//audioSys->playMenuClick();
	switch (currentGameState)
	{
	case 0:	// GAME_SCENE
	{
		break;
	}
	default:
		break;
	}
}

void Game::SetAmbientLight(float strength, glm::vec3 colour)
{
	ambientStr = strength;
	ambientColour = colour;

	ambient = ambientStr * ambientColour;
}

textureDetails Game::CreateTexture(std::string _filePath, GLint _textureWrapType)
{
	// Check if texture already exists
	for (auto it = textureMap.begin(); it != textureMap.end(); ++it)
	{
		// If texture already exists, return this texture
		if (it->first == _filePath)
		{
			return it->second;
		}
	}
	// else texture doesn't exist
	textureDetails details;
	Texture* newTex;
	newTex = new Texture(_filePath, _textureWrapType);
	newTex->init();
	details.texture = newTex;
	details.tex = newTex->GetTexture();
	textureMap.insert(std::pair<std::string, textureDetails>(_filePath, details));

	return details;
}

Model * Game::LoadModel(std::string _filePath, lightInfo _lighting)
{
	// Check if model already exists
	for (auto it = modelMap.begin(); it != modelMap.end(); ++it)
	{
		// Model has already been loaded before, return a pointer to model
		if (it->first == _filePath)
		{
			return it->second;
		}
	}
	// else model has not been loaded before
	Model* model;
	model = new Model(_filePath, camera, _lighting);
	modelMap.insert(std::pair<std::string, Model*>(_filePath, model));

	return model;
}

void Game::CreateTextLabels()
{
	// SCORE LABEL EXAMPLE
	// Score
	//scoreText = new TextLabel(score + std::to_string(scoreNum), "Resources/Fonts/R-2014.ttf", glm::vec2(-380.0f, 265.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	//scoreText->SetScale(0.5f);
	
}

void Game::PopulateCubemapFilePathVec()
{
	// Only need to use filename. CubeMap already uses directory prefix
	std::string filePath;

	// Right
	// Starscape
	filePath = "right.jpg";
	vecCubeMapFilePaths.push_back(filePath);

	// Left
	filePath = "left.jpg";
	vecCubeMapFilePaths.push_back(filePath);

	// Top
	filePath = "top.jpg";
	vecCubeMapFilePaths.push_back(filePath);

	// Bottom
	filePath = "bottom.jpg";
	vecCubeMapFilePaths.push_back(filePath);

	// Back
	filePath = "back.jpg";
	vecCubeMapFilePaths.push_back(filePath);

	// Front
	filePath = "front.jpg";
	vecCubeMapFilePaths.push_back(filePath);
}

void Game::Restart()
{
	mainPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	camera->SetCamPos(glm::vec3(0.0f, 1.5f, -5.0f));
	camera->SetFOVY(95.0f);

	terrain->Init();

	float newTerrainHeight = terrain->GetHeight(0.0f, 0.0f) + (cube->GetHeight() * 0.5f);
	fountainParticleSys->SetOrigin(glm::vec3(0.0f, newTerrainHeight, 0.0f));

	outlineObjects = false;
	wireframeOn = false;
	scissorTestOn = false;
}

void Game::MovePlayerIfInTerrainBounds(glm::vec3 playerAddedMovement)
{
	// Check if the player is walking on terrain
	float futurePlayerX, futurePlayerZ;

	futurePlayerX = mainPlayer->GetPosition().x + playerAddedMovement.x;
	futurePlayerZ = mainPlayer->GetPosition().z + playerAddedMovement.z;

	if ((futurePlayerX < terrain->GetWidth() * 0.49f) && (futurePlayerX > -terrain->GetWidth() * 0.49f))
	{
		if ((futurePlayerZ < terrain->GetWidth() * 0.49f) && (futurePlayerZ > -terrain->GetWidth() * 0.49f))
		{
			mainPlayer->IncrementPositionVec(playerAddedMovement);
		}
	}
}

// Header Files
//=============

#include "cMyGame.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/Graphics/Camera.h"
#include "../../Engine/Math/Functions.h"
#include "../../Engine/Graphics/UIHandler.h"
#include <iostream>
// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cMyGame::~cMyGame()
{

}

// Inherited Implementation
//=========================

// Initialization / Clean Up
//--------------------------

bool eae6320::cMyGame::Initialize()
{
	currentAngle = 0.174533f;
	m_currentRow = 0;
	eae6320::Graphics::Mesh* Plane = new eae6320::Graphics::Mesh();
	eae6320::Graphics::Mesh* floor = new eae6320::Graphics::Mesh();
	eae6320::Graphics::RenderData* r_plane = new eae6320::Graphics::RenderData();
	eae6320::Graphics::RenderData* r_floor = new eae6320::Graphics::RenderData();
	m_currentCamera = new eae6320::Graphics::Camera();
	eae6320::Graphics::setCurrentCamera(m_currentCamera);
	eae6320::Graphics::Material* planeMat  = new eae6320::Graphics::Material();
	eae6320::Graphics::Material* floorMat = new eae6320::Graphics::Material();
	eae6320::Graphics::Material* spriteMat = new eae6320::Graphics::Material();
	spriteMat->load("data/materials/material3.mat");
	float top = -0.7f;
	float bottom = -0.9f;
	float right = 0.9f;
	float left = 0.7f;
	float height = spriteMat->getTextureHeight();
	float width = spriteMat->getTextureWidth();
	eae6320::Graphics::UIHandler::convertPointToResoultion(left,right,bottom,top,height,width);
	eae6320::Graphics::cSprite* sprite = new eae6320::Graphics::cSprite(left,right,top,bottom,0.0f,1.0f,1.0f,0.0f);
	sprite->Initialize();
	eae6320::Graphics::SpriteRenderData* spriteData = new eae6320::Graphics::SpriteRenderData();
	spriteData->sprite = sprite;
	spriteData->material = spriteMat;
	listOfSpriteRenderData.push_back(spriteData);
	planeMat->load("data/materials/material1.mat");
	floorMat->load("data/materials/material2.mat");
	if (Plane->initialize("data/meshes/Plane.mesh"))
	{
		r_plane->mesh = Plane;
		r_plane->position.x = 0.0f;
		r_plane->position.y = 0.0f;
		r_plane->position.z = 0.0f;
		r_plane->material = planeMat;
		listOfRenderData.push_back(r_plane);
	}
	else
		delete Plane;
	if (floor->initialize("data/meshes/floor.mesh"))
	{
		r_floor->mesh = floor;
		r_floor->position.x = 0.2f;
		r_floor->position.y = -1.0f;
		r_floor->position.z = 0.0f;
		r_floor->material = floorMat;
		listOfRenderData.push_back(r_floor);
	}
	else
		delete floor;
	eae6320::Math::cVector yaxis(0, 1, 0);
	listOfRenderData[0]->rotation = eae6320::Math::cQuaternion(Math::ConvertDegreesToRadians(20), yaxis);
	listOfRenderData[1]->rotation = eae6320::Math::cQuaternion(Math::ConvertDegreesToRadians(20), yaxis);
	return true;

}

float eae6320::cMyGame::getInitialAngle()
{
	
	int random = rand() % 4 + 1;
	float currentAngle = 0;
	
	switch (random)
	{
	case 1 :
		currentAngle = 0;
		break;
	case 2:
		currentAngle = 90;
		break;
	case 3:
		currentAngle = 180;
		break;
	case 4:
		currentAngle = 270;
		break;
	}
	
	return currentAngle;
}

void eae6320::cMyGame::moveCurrentRightRow()
{
	currentAngles[m_currentRow] += 90 ;
	if (currentAngles[m_currentRow] > 270)
		currentAngles[m_currentRow] = 0;
	eae6320::Math::cVector yaxis(0, 1, 0);
	listOfRenderData[m_currentRow]->rotation = eae6320::Math::cQuaternion(Math::ConvertDegreesToRadians(currentAngles[m_currentRow]), yaxis);
}
void eae6320::cMyGame::moveCurrentLeftRow()
{
	currentAngles[m_currentRow] -= 90;
	if (currentAngles[m_currentRow] < 0)
		currentAngles[m_currentRow] = 270;
	eae6320::Math::cVector yaxis(0, 1, 0);
	listOfRenderData[m_currentRow]->rotation = eae6320::Math::cQuaternion(Math::ConvertDegreesToRadians(currentAngles[m_currentRow]), yaxis);
}

bool eae6320::cMyGame::CleanUp()
{ 
	for (unsigned int i = 0; i < listOfRenderData.size();i++)
	{
	
	listOfRenderData[i]->material->cleanUp();
	delete listOfRenderData[i]->material;
	delete listOfRenderData[i]->mesh;
	delete listOfRenderData[i];
	}
	listOfRenderData.clear();
	if (m_currentCamera)
		delete m_currentCamera;
	return true;
}
 void eae6320::cMyGame::Update()
{
	float cameraOffset[2] = { 0.0f,0.0f };
	{
		if (eae6320::UserInput::IsKeyPressed(0x57))
			cameraOffset[1] += 1.0f;
		if (eae6320::UserInput::IsKeyPressed(0x53))
			cameraOffset[1] -= 1.0f;
		if (eae6320::UserInput::IsKeyPressed(0x41))
			cameraOffset[0] -= 1.0f;
		if (eae6320::UserInput::IsKeyPressed(0x44))
			cameraOffset[0] += 1.0f;
	}
	{
		const float speed_unitsPerSecond = 1.0f;
		const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
		cameraOffset[0] *= offsetModifier;
		cameraOffset[1] *= offsetModifier;
		eae6320::Math::cVector postion = m_currentCamera->getPosition();
		postion.x += cameraOffset[0];
		postion.z += cameraOffset[1];
		m_currentCamera->setPosition(postion);

	}
	updateActorsList(listOfRenderData, listOfSpriteRenderData);
}
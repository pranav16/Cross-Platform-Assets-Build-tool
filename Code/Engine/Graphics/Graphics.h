/*
	This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Header Files
//=============

#include "Configuration.h"
#include <vector>
#include "Mesh.h"
#if defined( EAE6320_PLATFORM_WINDOWS )
	#include "../Windows/Includes.h"
#endif
#include "ConstantBuffer.h"

#include "Camera.h"
#include "cSprite.h"
#include "Material.h"
// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		// Render
		//-------
		struct sVertex;
		struct sConstantBufferFrameData;
		struct sDrawCall;
		struct GraphicsPlatformData;
		class Camera;
		struct RenderData
		{
			Mesh* mesh;
			Material* material;
			eae6320::Math::cVector position;
			eae6320::Math::cQuaternion rotation;

		};
		struct SpriteRenderData
		{
			cSprite* sprite;
			Material* material;
	
		};
		struct sDrawData
		{
			sDrawCall s_drawBuffer;
			Material* material;
			Mesh* mesh;
		};
		struct sSpriteDrawData
		{
			sDrawCall s_drawBuffer;
			Material* material;
			cSprite* sprite;

		};

		extern ConstantBuffer s_frameConstantbuffer;
		extern ConstantBuffer s_drawCallConstantbuffer;
		extern Camera* m_currentCamera;
		


		extern std::vector<sDrawData> s_listOfDrawData;
		extern std::vector<sSpriteDrawData> s_listOfSpriteDrawData;
	    
		bool setCurrentCamera(Camera* i_camera);
		
		extern sConstantBufferFrameData s_FrameConstantBufferData;
		void BeginRenderFrame();
		void EndRenderFrame();
		void SubmitDrawData(std::vector<RenderData*>i_listOfRenderables,std::vector<SpriteRenderData*>i_listOfSprites);
		void RenderFrame();
		
		// Initialization / Clean Up
		//--------------------------

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow;
	#if defined( EAE6320_PLATFORM_D3D )
			unsigned int resolutionWidth, resolutionHeight;
	#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication;
	#endif
#endif
		};
		
		bool Initialize( const sInitializationParameters& i_initializationParameters );
		bool CleanUp();
	
		
	}
}

#endif	// EAE6320_GRAPHICS_H

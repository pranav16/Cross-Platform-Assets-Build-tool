#pragma once
#ifndef _GRAPHICS_DATA_H
#define _GRAPHICS_DATA_H
#include <cinttypes>
#include "../Math/cMatrix_transformation.h"

namespace eae6320
{

	namespace Graphics
	{
		struct sVertex
		{
			// POSITION
			// 2 floats == 8 bytes
			// Offset = 0
			float x, y,z,u,v;
			uint8_t r, g, b, a;
		};
		struct sIndex
		{
			uint16_t index1, index2, index3;
		};
		
		struct sConstantBufferFrameData
		{
			Math::cMatrix_transformation g_transform_worldToCamera;
			Math::cMatrix_transformation g_transform_cameraToScreen;
			union
			{
				float g_elapsedSecondCount_total;
				float register8[4];
			};
		};
		struct sDrawCall
		{
			Math::cMatrix_transformation g_transform_localToWorld;
		};
	   
		struct sMaterialConstants
		{
			float r, g, b, a;
			sMaterialConstants()
			{
				r = 1.0f;
				g = 1.0f;
				b = 1.0f;
				a = 1.0f;
			};
		};
	

	}//namspace Graphics

}//namespace eae6320


#endif // _GRAPHICS_DATA_H
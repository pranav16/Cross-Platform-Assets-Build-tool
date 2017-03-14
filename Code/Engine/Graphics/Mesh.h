#pragma once
#ifndef  _MESH_H_
#define _MESH_H_
#include "GraphicsDataTypes.h"

namespace eae6320
{

	namespace Graphics
	{
		
		struct GraphicsPlatformData;
		
		
	
		class Mesh
		{
		public:
			Mesh();
			~Mesh();
			bool initialize(const char* filePath);
			
			void draw();
			bool cleanUp();

		private:
		
			bool intializeVertexBuffer();
			bool initialize(uint16_t vertexCount, sVertex* listOfVertcies, uint16_t indexCount, sIndex* listofIndicies);
			sVertex* m_listOfVerticies;
			sIndex* m_listOfIndicies;
			//struct that constains platformspecfic Data
			GraphicsPlatformData* m_platformData;
		    
			uint16_t m_vertexCount;
			uint16_t m_indexCount;
		
		};

	} //namespace Graphics
} // namespace eae6320





#endif // ! _MESH_H_

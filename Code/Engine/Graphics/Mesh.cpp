#include "Mesh.h"
#include <iostream>
#include "../Logging/Logging.h"
#include "../Asserts/Asserts.h"
#include "../Platform/Platform.h"
#include <iostream>
#include <stdio.h>
eae6320::Graphics::Mesh::Mesh()
{

}

eae6320::Graphics::Mesh::~Mesh()
{

}
bool eae6320::Graphics::Mesh::initialize(const char* filePath)
{
	
		bool whereThereErrors = false;
		uint16_t indexCount = 0;
		uint16_t vertexCount = 0;
		eae6320::Graphics::sVertex* vertex = nullptr;
		eae6320::Graphics::sIndex* indicies = nullptr;
		std::string errorMsg;
		Platform::sDataFromFile dataFromFile;
		whereThereErrors = Platform::LoadBinaryFile(filePath, dataFromFile,&errorMsg);
		whereThereErrors = !whereThereErrors;
		if (!whereThereErrors)
			Logging::OutputError(errorMsg.c_str());
		uint8_t* data = reinterpret_cast<uint8_t*>(dataFromFile.data);
		indexCount = *reinterpret_cast<uint16_t*>(data);
		data += sizeof(uint16_t);
		vertexCount = *reinterpret_cast<uint16_t*>(data);
		data += sizeof(uint16_t);
		vertex = reinterpret_cast<eae6320::Graphics::sVertex*>(data);
		data += (sizeof(eae6320::Graphics::sVertex) * vertexCount);
		indicies = reinterpret_cast<eae6320::Graphics::sIndex*>(data);
		//m_vertexCount = vertexCount;
		if (!whereThereErrors)
			initialize(vertexCount, vertex, indexCount, indicies);

	return !whereThereErrors;
}



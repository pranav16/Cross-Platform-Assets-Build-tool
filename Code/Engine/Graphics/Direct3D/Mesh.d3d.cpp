#include "../Mesh.h"

#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "DirectXRefrences.d3d.h"
#include "../GraphicsDataTypes.h"

namespace eae6320
{

	namespace Graphics
	{
		struct GraphicsPlatformData
		{
			ID3D11Buffer* s_vertexBuffer;
			ID3D11Buffer* s_indexBuffer;
		};



	}//namspace Graphics

}//namespace eae6320


bool  eae6320::Graphics::Mesh::initialize(uint16_t vertexCount, sVertex* listOfVertcies, uint16_t indexCount, sIndex* listofIndicies)
{
	bool status = false;
	m_vertexCount = vertexCount;
	m_listOfVerticies = listOfVertcies;
	m_indexCount = indexCount;
	m_listOfIndicies = listofIndicies;
	for (int i = 0; i < indexCount; i++)
	{
		uint16_t ind = listofIndicies[i].index1;
		uint16_t in = listofIndicies[i].index2;
		uint16_t jp = listofIndicies[i].index3;
	}
	status = intializeVertexBuffer();
	return status;
}



bool eae6320::Graphics::Mesh::intializeVertexBuffer()
{
	// These elements must match the VertexFormat::sVertex layout struct exactly.
	// They instruct Direct3D how to match the binary data in the vertex buffer
	// to the input elements in a vertex shader
	// (by using so-called "semantic" names so that, for example,
	// "POSITION" here matches with "POSITION" in shader code).
	// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
	m_platformData = new eae6320::Graphics::GraphicsPlatformData;

// Eventually the vertex data should come from a file but for now it is hard-coded here.
// You will have to update this in a future assignment
// (one of the most common mistakes in the class is to leave hard-coded values here).

	{
		const unsigned int vertexCount = m_vertexCount;
		const unsigned int bufferSize = vertexCount * sizeof(sVertex);

		D3D11_BUFFER_DESC bufferDescription = { 0 };
		{
			bufferDescription.ByteWidth = bufferSize;
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used
		}
		D3D11_SUBRESOURCE_DATA initialData = { 0 };
		{
			initialData.pSysMem = m_listOfVerticies;
			// (The other data members are ignored for non-texture buffers)
		}

		const HRESULT result = g_direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &(m_platformData->s_vertexBuffer));
		if (FAILED(result))
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create the vertex buffer with HRESULT %#010x", result);
			return false;
		}
	}
//index buffer
{
	const unsigned int indexCount = m_indexCount;
	const unsigned int bufferSize = indexCount * sizeof(sIndex);
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	{
		bufferDescription.ByteWidth = bufferSize;
		bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
		bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
		bufferDescription.MiscFlags = 0;
		bufferDescription.StructureByteStride = 0;	// Not used
	}
	D3D11_SUBRESOURCE_DATA initialData = { 0 };
	{
		initialData.pSysMem = m_listOfIndicies;
		// (The other data members are ignored for non-texture buffers)
	}
	const HRESULT result = g_direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &(m_platformData->s_indexBuffer));
	if (FAILED(result))
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create the vertex buffer with HRESULT %#010x", result);
		return false;
	}

}

return true;
}



void eae6320::Graphics::Mesh::draw()
{
	EAE6320_ASSERTF(m_platformData != nullptr, "calling draw when init has either failed or not called");
	
	// Bind a specific vertex buffer to the device as a data source
	{
		const unsigned int startingSlot = 0;
		const unsigned int vertexBufferCount = 1;
		// The "stride" defines how large a single vertex is in the stream of data
		const unsigned int bufferStride = sizeof(sVertex);
		// It's possible to start streaming data in the middle of a vertex buffer
		const unsigned int bufferOffset = 0;
		g_direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &(m_platformData->s_vertexBuffer), &bufferStride, &bufferOffset);
	}

	
	// Bind a specific vertex buffer to the device as a data source
	{
		EAE6320_ASSERT(m_platformData->s_indexBuffer != NULL);
		// Every index is a 16 bit unsigned integer
		const DXGI_FORMAT format = DXGI_FORMAT_R16_UINT;
		// The indices start at the beginning of the buffer
		const unsigned int offset = 0;
		g_direct3dImmediateContext->IASetIndexBuffer(m_platformData->s_indexBuffer, format, offset);
	}

	// Render triangles from the currently-bound vertex and index buffers
	{
		// It's possible to start rendering primitives in the middle of the stream
		const unsigned int indexOfFirstIndexToUse = 0;
		const unsigned int offsetToAddToEachIndex = 0;
		const unsigned int indexCount = m_indexCount * 3;
		g_direct3dImmediateContext->DrawIndexed(indexCount, indexOfFirstIndexToUse, offsetToAddToEachIndex);
	}
	

}

bool eae6320::Graphics::Mesh::cleanUp()
{
	bool wereThereErrors = false;
	m_platformData->s_vertexBuffer->Release();
	m_platformData->s_vertexBuffer = nullptr;
	m_platformData->s_indexBuffer->Release();
	m_platformData->s_indexBuffer = nullptr;
	delete m_platformData;
	m_platformData = nullptr;
	delete[] m_listOfVerticies;
	m_listOfVerticies = nullptr;
	m_vertexCount = 0;
	
	return !wereThereErrors;

}

#include "DirectXRefrences.d3d.h"
#include "../ConstantBuffer.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../../Math/Functions.h"
namespace eae6320
{
	namespace Graphics
	{
		struct ConstantBufferPlatformData
		{
			ID3D11Buffer* constantBuffer;

		};
	}

}

bool eae6320::Graphics::ConstantBuffer::initialize(const ConstantBufferTypes type,const size_t sizeOfBuffer,const void* payload)
{
	bool wereThereErrors = false;
	m_type = type;
	m_size = sizeOfBuffer;
	m_platfromData = new ConstantBufferPlatformData;
	D3D11_BUFFER_DESC bufferDescription = { 0 };
	{
		// The byte width must be rounded up to a multiple of 16
		bufferDescription.ByteWidth = Math::RoundUpToMultiple_powerOf2(static_cast<unsigned int>(m_size), 16u);
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;	// The CPU must be able to update the buffer
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// The CPU must write, but doesn't read
		bufferDescription.MiscFlags = 0;
		bufferDescription.StructureByteStride = 0;	// Not used
	}
	D3D11_SUBRESOURCE_DATA initialData = { 0 };
	{
		// Fill in the constant buffer
		
		initialData.pSysMem = payload;
		// (The other data members are ignored for non-texture buffers)
	}

	const HRESULT result = g_direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &m_platfromData->constantBuffer);
	if (SUCCEEDED(result))
	{
		return true;
	}
	else
	{
		EAE6320_ASSERT(false);
		eae6320::Logging::OutputError("Direct3D failed to create a constant buffer with HRESULT %#010x", result);
		return false;
	}



	return !wereThereErrors;
}

bool eae6320::Graphics::ConstantBuffer::bindBuffers()
{

	// bind the constant buffer to the shader
	{
		
			const unsigned int registerAssignedInShader = m_type;
			const unsigned int bufferCount = 1;
			g_direct3dImmediateContext->VSSetConstantBuffers(registerAssignedInShader, bufferCount, &m_platfromData->constantBuffer);
			g_direct3dImmediateContext->PSSetConstantBuffers(registerAssignedInShader, bufferCount, &m_platfromData->constantBuffer);
		
	}
	return true;
}
bool eae6320::Graphics::ConstantBuffer::UpdateData(const void* payLoad)
{
	
	
	// Get a pointer from Direct3D that can be written to
	void* memoryToWriteTo = NULL;
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		{
			// Discard previous contents when writing
			const unsigned int noSubResources = 0;
			const D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
			const unsigned int noFlags = 0;
			const HRESULT result = g_direct3dImmediateContext->Map(m_platfromData->constantBuffer, noSubResources, mapType, noFlags, &mappedSubResource);
			if (SUCCEEDED(result))
			{
				memoryToWriteTo = mappedSubResource.pData;
			}
			else
			{
				EAE6320_ASSERT(false);
			}
		}
	}
	if (memoryToWriteTo)
	{
		// Copy the new data to the memory that Direct3D has provided
		memcpy(memoryToWriteTo, payLoad,m_size);
		// Let Direct3D know that the memory contains the data
		// (the pointer will be invalid after this call)
		const unsigned int noSubResources = 0;
		g_direct3dImmediateContext->Unmap(m_platfromData->constantBuffer, noSubResources);
		memoryToWriteTo = NULL;
	}
	return true;
}

bool eae6320::Graphics::ConstantBuffer::cleanUp()
{
	bool wereThereErrors = false;
	if ( m_platfromData->constantBuffer)
	{
		m_platfromData->constantBuffer->Release();
		m_platfromData->constantBuffer = NULL;
	}

	delete m_platfromData;
	m_size = 0;
	return !wereThereErrors;
}
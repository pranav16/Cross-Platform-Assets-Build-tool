#include  "../Effects.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "DirectXRefrences.d3d.h"
#include "../GraphicsDataTypes.h"
#include "../cRenderState.h"

namespace eae6320
{
	namespace Graphics
	{
		struct sEffectsData
		{
			ID3D11VertexShader* m_vertexShader;
			ID3D11PixelShader* m_pixelShader;
			ID3D11InputLayout* m_inputLayout;
			cRenderState renderState;
		};

	}
}
bool  eae6320::Graphics::Effects::load(const char* effectsPath)
{

	bool whereThereErrors = false;
	std::string errorMsg;
	Platform::sDataFromFile dataFromFile;
	whereThereErrors = Platform::LoadBinaryFile(effectsPath, dataFromFile, &errorMsg);
	whereThereErrors = !whereThereErrors;
	if (!whereThereErrors)
		Logging::OutputError(errorMsg.c_str());
	uint8_t* data = reinterpret_cast<uint8_t*>(dataFromFile.data);
	uint8_t* renderStates = data;
	data += sizeof(uint8_t);
	uint16_t vertexPathSize = *reinterpret_cast<uint16_t*> (data);
	data += sizeof(uint16_t);
	char* _vertexShaderPath = reinterpret_cast<char*>(data);
	data += vertexPathSize;
	uint16_t fragmentShaderPath = *reinterpret_cast<uint16_t*>(data);
	data += sizeof(uint16_t);
	char* fragmentShader = reinterpret_cast<char*>(data);
	m_effectsData = new sEffectsData;
	

	return LoadFragmentShader(fragmentShader) && LoadVertexShader(_vertexShaderPath) && m_effectsData->renderState.Initialize(*renderStates);

}


bool eae6320::Graphics::Effects::cleanUp()
{
	bool whereThereErrors = false;
	if (m_effectsData)
	{
		if(m_effectsData->m_inputLayout)
		m_effectsData->m_inputLayout->Release();
		if(m_effectsData->m_pixelShader)
		m_effectsData->m_pixelShader->Release();
		if(m_effectsData->m_vertexShader->Release())
		m_effectsData->m_vertexShader->Release();
		delete m_effectsData;
		m_effectsData = nullptr;
	}

	return !whereThereErrors;
}
bool eae6320::Graphics::Effects::bind()
{
	bool whereThereErrors = false;
	{
		ID3D11ClassInstance** const noInterfaces = NULL;
		const unsigned int interfaceCount = 0;
		{
		
			g_direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		g_direct3dImmediateContext->VSSetShader(m_effectsData->m_vertexShader, noInterfaces, interfaceCount);
		g_direct3dImmediateContext->PSSetShader(m_effectsData->m_pixelShader, noInterfaces, interfaceCount);
		m_effectsData->renderState.Bind();
	}
	return !whereThereErrors;
}
bool eae6320::Graphics::Effects::LoadFragmentShader(const char* fragmentShaderPath)
{
	bool wereThereErrors = false;

	// Load the compiled shader
	eae6320::Platform::sDataFromFile compiledShader;
	const char* const path = fragmentShaderPath;
	{
		std::string errorMessage;
		if (!eae6320::Platform::LoadBinaryFile(path, compiledShader, &errorMessage))
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, errorMessage.c_str());
			eae6320::Logging::OutputError("Failed to load the shader file \"%s\": %s", path, errorMessage.c_str());
			goto OnExit;
		}
	}
	// Create the shader object
	{
		
		ID3D11ClassLinkage* const noInterfaces = NULL;
		const HRESULT result = g_direct3dDevice->CreatePixelShader(
			compiledShader.data, compiledShader.size, noInterfaces, &(m_effectsData->m_pixelShader));
		if (FAILED(result))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create the shader %s with HRESULT %#010x", path, result);
			goto OnExit;
		}
	}

OnExit:

	compiledShader.Free();

	return !wereThereErrors;
}


bool eae6320::Graphics::Effects::LoadVertexShader(const char* vertexShaderPath)
{
	{
		bool wereThereErrors = false;

		// Load the compiled shader
		eae6320::Platform::sDataFromFile* compiledShader = new eae6320::Platform::sDataFromFile;

		const char* const path = vertexShaderPath;
		{
			std::string errorMessage;
			if (!eae6320::Platform::LoadBinaryFile(path, *compiledShader, &errorMessage))
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, errorMessage.c_str());
				eae6320::Logging::OutputError("Failed to load the shader file \"%s\": %s", path, errorMessage.c_str());
				goto OnExit;
			}
		}
		
		// Create the shader object
		{
			ID3D11ClassLinkage* const noInterfaces = NULL;
			const HRESULT result = g_direct3dDevice->CreateVertexShader(
				compiledShader->data, compiledShader->size, noInterfaces, &(m_effectsData->m_vertexShader));
			if (FAILED(result))
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Direct3D failed to create the shader %s with HRESULT %#010x", path, result);
				goto OnExit;
			}
		}
		createInputLayout(compiledShader);
		//compiledShader->Free();
		//delete compiledShader;
	OnExit:

		return !wereThereErrors;
	}
	
}
bool eae6320::Graphics::Effects::createInputLayout(eae6320::Platform::sDataFromFile* compiledShader)
{
	const unsigned int vertexElementCount = 3;
	D3D11_INPUT_ELEMENT_DESC layoutDescription[vertexElementCount] = { 0 };
	{
		// Slot 0

		// POSITION
		// 2 floats == 8 bytes
		// Offset = 0
		{
			D3D11_INPUT_ELEMENT_DESC& positionElement = layoutDescription[0];

			positionElement.SemanticName = "POSITION";
			positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
			positionElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			positionElement.InputSlot = 0;
			positionElement.AlignedByteOffset = offsetof(sVertex, x);
			positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
		}
		{
			D3D11_INPUT_ELEMENT_DESC& positionElement = layoutDescription[1];

			positionElement.SemanticName = "COLOR";
			positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
			positionElement.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			positionElement.InputSlot = 0;
			positionElement.AlignedByteOffset = offsetof(sVertex, r);
			positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
		}
		{
			D3D11_INPUT_ELEMENT_DESC& positionElement = layoutDescription[2];

			positionElement.SemanticName = "TEXCOORD";
			positionElement.SemanticIndex = 0;	// (Semantics without modifying indices at the end can always use zero)
			positionElement.Format = DXGI_FORMAT_R32G32_FLOAT;
			positionElement.InputSlot = 0;
			positionElement.AlignedByteOffset = offsetof(sVertex, u);
			positionElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			positionElement.InstanceDataStepRate = 0;	// (Must be zero for per-vertex data)
		}
	}
	{
		const HRESULT result = g_direct3dDevice->CreateInputLayout(layoutDescription, vertexElementCount,
			compiledShader->data, compiledShader->size, &(m_effectsData->m_inputLayout));
		if (FAILED(result))
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create a vertex input layout with HRESULT %#010x", result);
			return false;
		}
		// Specify what kind of data the vertex buffer holds
		{
			// Set the layout (which defines how to interpret a single vertex)
			g_direct3dImmediateContext->IASetInputLayout(m_effectsData->m_inputLayout);
			// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
			// we have defined the vertex buffer as a triangle list
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			g_direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
	}
	return true;
}

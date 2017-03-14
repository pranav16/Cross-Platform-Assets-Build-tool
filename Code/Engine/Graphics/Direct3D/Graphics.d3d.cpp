// Header Files
//=============

#include "../Graphics.h"

#include <cstddef>
#include <cstdint>
#include <D3D11.h>
#include <D3DX11async.h>
#include <D3DX11core.h>
#include <DXGI.h>
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../../Time/Time.h"
#include "DirectXRefrences.d3d.h"
#include "../GraphicsDataTypes.h"
#include "../../Platform/Platform.h"

//===========================

	HWND s_renderingWindow = NULL;
	// These are D3D interfaces
	ID3D11Device* s_direct3dDevice = NULL;
	IDXGISwapChain* s_swapChain = NULL;
	ID3D11DeviceContext* s_direct3dImmediateContext = NULL;
	ID3D11RenderTargetView* s_renderTargetView = NULL;
	ID3D11DepthStencilView* s_DepthStencil = NULL;
	ID3D10Blob* compiledVertexShader = NULL;
	ID3D11SamplerState* s_samplerState = NULL;
	
	// This struct determines the layout of the geometric data that the CPU will send to the GPU

	// D3D has an "input layout" object that associates the layout of the struct above
	// with the input from a vertex shader
	

	

	// The vertex shader is a program that operates on vertices.
	// Its input comes from a C/C++ "draw call" and is:
	//	* Position
	//	* Any other data we want
	// Its output is:
	//	* Position
	//		(So that the graphics hardware knows which pixels to fill in for the triangle)
	//	* Any other data we want
	ID3D11VertexShader* s_vertexShader = NULL;
	// The fragment shader is a program that operates on fragments
	// (or potential pixels).
	// Its input is:
	//	* The data that was output from the vertex shader,
	//		interpolated based on how close the fragment is
	//		to each vertex in the triangle.
	// Its output is:
	//	* The final color that the pixel should be
	ID3D11PixelShader* s_fragmentShader = NULL;

	


// Helper Function Declarations
//=============================

namespace
{

	bool CreateDevice( const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight );
	bool CreateViews( const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight );
	bool setupSampler();

}

// Interface
//==========

// Render
//-------



void eae6320::Graphics::BeginRenderFrame()
{

	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		// Black is usually used
		float clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
		s_direct3dImmediateContext->ClearRenderTargetView( s_renderTargetView, clearColor);
		const float depthToClear = 1.0f;
		const uint8_t stencilToClear = 0; // Arbitrary until stencil is used
		s_direct3dImmediateContext->ClearDepthStencilView(s_DepthStencil, D3D11_CLEAR_DEPTH,
			depthToClear, stencilToClear);
	}
	
	 //update the constant buffer
	{
		// Update the struct (i.e. the memory that we own)
		s_FrameConstantBufferData.g_elapsedSecondCount_total = Time::GetElapsedSecondCount_total();
		s_FrameConstantBufferData.g_transform_cameraToScreen = m_currentCamera->getCameraToScreen();
		s_FrameConstantBufferData.g_transform_worldToCamera = m_currentCamera->getWorldToCamera();
		// Get a pointer from Direct3D that can be written to
		//s_frameConstantbuffer.bindBuffers();
	}
	s_frameConstantbuffer.UpdateData(&s_FrameConstantBufferData);

	// Draw the geometry
	{
		// Set the vertex and fragment shaders
	
	
	}

}
void eae6320::Graphics::EndRenderFrame()
{
	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (to the front buffer)
	{
		const unsigned int swapImmediately = 0;
		const unsigned int presentNextFrame = 0;
		const HRESULT result = s_swapChain->Present(swapImmediately, presentNextFrame);
		EAE6320_ASSERT(SUCCEEDED(result));
	}


}


// Initialization / Clean Up
//--------------------------

bool eae6320::Graphics::Initialize( const sInitializationParameters& i_initializationParameters )
{
	bool wereThereErrors = false;
	sDrawData data;
	s_renderingWindow = i_initializationParameters.mainWindow;


	// Create an interface to a Direct3D device
	if ( !CreateDevice( i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}
	g_direct3dDevice = s_direct3dDevice;
	g_direct3dImmediateContext = s_direct3dImmediateContext;

	if (!setupSampler())
	{
		wereThereErrors = true;
		goto OnExit;
	}
	// Initialize the viewport of the device
	if ( !CreateViews( i_initializationParameters.resolutionWidth, i_initializationParameters.resolutionHeight ) )
	{
		wereThereErrors = true;
		goto OnExit;
	}

	if (!s_frameConstantbuffer.initialize(ConstantBufferTypes::FrameData, sizeof(s_FrameConstantBufferData), &s_FrameConstantBufferData))
	{
		wereThereErrors = true;
		goto OnExit;
	}
	
	s_listOfDrawData.push_back(data);
	if (!s_drawCallConstantbuffer.initialize(ConstantBufferTypes::DrawCallData, sizeof(sDrawCall),&s_listOfDrawData[0]))
	{
		EAE6320_ASSERT(false);
		return false;
	}
	
OnExit:

	// A vertex shader object is used to render the triangle.
	// The compiled vertex shader is the actual compiled code,
	// and once it has been used to create the vertex input layout
	// it can be freed.
	if (!wereThereErrors)
	{
		
		
	
			s_frameConstantbuffer.bindBuffers();
			s_drawCallConstantbuffer.bindBuffers();
		
	}

	return !wereThereErrors;
}

bool eae6320::Graphics::CleanUp()
{
	bool wereThereErrors = false;

	if ( s_direct3dDevice )
	{


		s_frameConstantbuffer.cleanUp();
		s_drawCallConstantbuffer.cleanUp();

		if ( s_renderTargetView )
		{
			s_renderTargetView->Release();
			s_renderTargetView = NULL;
		}
		if (s_samplerState)
		{
			s_samplerState->Release();
			s_samplerState = nullptr;
		}
		if (s_DepthStencil)
		{
			s_DepthStencil->Release();
			s_DepthStencil = NULL;
		}
		if (compiledVertexShader)
		{
			compiledVertexShader->Release();
			compiledVertexShader = NULL;
		}
		s_direct3dDevice->Release();
		s_direct3dDevice = NULL;
	}
	if ( s_direct3dImmediateContext )
	{
		s_direct3dImmediateContext->Release();
		s_direct3dImmediateContext = NULL;
	}
	if ( s_swapChain )
	{
		s_swapChain->Release();
		s_swapChain = NULL;
	}

	s_renderingWindow = NULL;
	g_direct3dDevice = NULL;
    g_direct3dImmediateContext = NULL;




	return !wereThereErrors;
}

// Helper Function Definitions
//============================

namespace
{

	bool CreateDevice(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
	{
		IDXGIAdapter* const useDefaultAdapter = NULL;
		const D3D_DRIVER_TYPE useHardwareRendering = D3D_DRIVER_TYPE_HARDWARE;
		const HMODULE dontUseSoftwareRendering = NULL;
		unsigned int flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
		{
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
			flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		}
		D3D_FEATURE_LEVEL* const useDefaultFeatureLevels = NULL;
		const unsigned int requestedFeatureLevelCount = 0;
		const unsigned int sdkVersion = D3D11_SDK_VERSION;
		DXGI_SWAP_CHAIN_DESC swapChainDescription = { 0 };
		{
			{
				DXGI_MODE_DESC& bufferDescription = swapChainDescription.BufferDesc;

				bufferDescription.Width = i_resolutionWidth;
				bufferDescription.Height = i_resolutionHeight;
				{
					DXGI_RATIONAL& refreshRate = bufferDescription.RefreshRate;

					refreshRate.Numerator = 0;	// Refresh as fast as possible
					refreshRate.Denominator = 1;
				}
				bufferDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				bufferDescription.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				bufferDescription.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			}
			{
				DXGI_SAMPLE_DESC& multiSamplingDescription = swapChainDescription.SampleDesc;

				multiSamplingDescription.Count = 1;
				multiSamplingDescription.Quality = 0;	// Anti-aliasing is disabled
			}
			swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDescription.BufferCount = 1;
			swapChainDescription.OutputWindow = s_renderingWindow;
			swapChainDescription.Windowed = TRUE;
			swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapChainDescription.Flags = 0;
		}
		D3D_FEATURE_LEVEL highestSupportedFeatureLevel;
		const HRESULT result = D3D11CreateDeviceAndSwapChain(useDefaultAdapter, useHardwareRendering, dontUseSoftwareRendering,
			flags, useDefaultFeatureLevels, requestedFeatureLevelCount, sdkVersion, &swapChainDescription,
			&s_swapChain, &s_direct3dDevice, &highestSupportedFeatureLevel, &s_direct3dImmediateContext);
		if (SUCCEEDED(result))
		{
			return true;
		}
		else
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create a Direct3D11 device with HRESULT %#010x", result);
			return false;
		}
	}

	
	bool CreateViews(const unsigned int i_resolutionWidth, const unsigned int i_resolutionHeight)
	{
		bool wereThereErrors = false;

		ID3D11Texture2D* backBuffer = NULL;
		ID3D11Texture2D* depthBuffer = NULL;

		// Create a "render target view" of the back buffer
		// (the back buffer was already created by the call to D3D11CreateDeviceAndSwapChain(),
		// but we need a "view" of it to use as a "render target",
		// meaning a texture that the GPU can render to)
		{
			// Get the back buffer from the swap chain
			{
				const unsigned int bufferIndex = 0; // This must be 0 since the swap chain is discarded
				const HRESULT result = s_swapChain->GetBuffer(bufferIndex, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to get the back buffer from the swap chain with HRESULT %#010x", result);
					goto OnExit;
				}
			}
			// Create the view
			{
				const D3D11_RENDER_TARGET_VIEW_DESC* const accessAllSubResources = NULL;
				const HRESULT result = s_direct3dDevice->CreateRenderTargetView(backBuffer, accessAllSubResources, &s_renderTargetView);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the render target view with HRESULT %#010x", result);
					goto OnExit;
				}
			}
		}
		// Create a depth/stencil buffer and a view of it
		{
			// Unlike the back buffer no depth/stencil buffer exists until and unless we create it
			{
				D3D11_TEXTURE2D_DESC textureDescription = { 0 };
				{
					textureDescription.Width = i_resolutionWidth;
					textureDescription.Height = i_resolutionHeight;
					textureDescription.MipLevels = 1; // A depth buffer has no MIP maps
					textureDescription.ArraySize = 1;
					textureDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth and 8 bits for stencil
					{
						DXGI_SAMPLE_DESC& sampleDescription = textureDescription.SampleDesc;
						sampleDescription.Count = 1; // No multisampling
						sampleDescription.Quality = 0; // Doesn't matter when Count is 1
					}
					textureDescription.Usage = D3D11_USAGE_DEFAULT; // Allows the GPU to write to it
					textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
					textureDescription.CPUAccessFlags = 0; // CPU doesn't need access
					textureDescription.MiscFlags = 0;
				}
				// The GPU renders to the depth/stencil buffer and so there is no initial data
				// (like there would be with a traditional texture loaded from disk)
				const D3D11_SUBRESOURCE_DATA* const noInitialData = NULL;
				const HRESULT result = s_direct3dDevice->CreateTexture2D(&textureDescription, noInitialData, &depthBuffer);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the depth buffer resource with HRESULT %#010x", result);
					goto OnExit;
				}
			}
			// Create the view
			{
				const D3D11_DEPTH_STENCIL_VIEW_DESC* const noSubResources = NULL;
				const HRESULT result = s_direct3dDevice->CreateDepthStencilView(depthBuffer, noSubResources, &s_DepthStencil);
				if (FAILED(result))
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Direct3D failed to create the depth stencil view with HRESULT %#010x", result);
					goto OnExit;
				}
			}
		}

		// Bind the views
		{
			const unsigned int renderTargetCount = 1;
			s_direct3dImmediateContext->OMSetRenderTargets(renderTargetCount, &s_renderTargetView, s_DepthStencil);
		}
		// Specify that the entire render target should be visible
		{
			D3D11_VIEWPORT viewPort = { 0 };
			viewPort.TopLeftX = viewPort.TopLeftY = 0.0f;
			viewPort.Width = static_cast<float>(i_resolutionWidth);
			viewPort.Height = static_cast<float>(i_resolutionHeight);
			viewPort.MinDepth = 0.0f;
			viewPort.MaxDepth = 1.0f;
			const unsigned int viewPortCount = 1;
			s_direct3dImmediateContext->RSSetViewports(viewPortCount, &viewPort);
		}

	OnExit:

		if (backBuffer)
		{
			backBuffer->Release();
			backBuffer = NULL;
		}
		if (depthBuffer)
		{
			depthBuffer->Release();
			depthBuffer = NULL;
		}

		return !wereThereErrors;
	}
	bool setupSampler()
	{
		D3D11_SAMPLER_DESC samplerStateDescription;
		{
			// Linear filtering
			samplerStateDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			// If UVs go outside [0,1] wrap them around (so that textures can tile)
			samplerStateDescription.AddressU = samplerStateDescription.AddressV
				= samplerStateDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			// Default values
			samplerStateDescription.MipLODBias = 0.0f;
			samplerStateDescription.MaxAnisotropy = 1;
			samplerStateDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerStateDescription.BorderColor[0] = samplerStateDescription.BorderColor[1]
				= samplerStateDescription.BorderColor[2] = samplerStateDescription.BorderColor[3] = 1.0f;
			samplerStateDescription.MinLOD = -FLT_MAX;
			samplerStateDescription.MaxLOD = FLT_MAX;
		}
		const HRESULT result = s_direct3dDevice->CreateSamplerState(&samplerStateDescription, &s_samplerState);
		if (FAILED(result))
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Direct3D failed to create a sampler state with HRESULT %#010x", result);
			return false;
		}

		// Bind the sampler state
		{
			const unsigned int samplerStateRegister = 0; // This must match the SamplerState register defined in the shader
			const unsigned int samplerStateCount = 1;
			s_direct3dImmediateContext->PSSetSamplers(samplerStateRegister, samplerStateCount, &s_samplerState);
		}
		return true;
	}
}
	

	

#pragma once
#ifndef  _DIRECTX_REFRENCES_H
#define _DIRECTX_REFRENCES_H

#include <cstddef>
#include <cstdint>
#include <D3D11.h>
#include <D3DX11async.h>
#include <D3DX11core.h>
#include <DXGI.h>
#include "../../Platform/Platform.h"
namespace eae6320
{

	namespace Graphics
	{

		// These are D3D interfaces
		extern ID3D11Device* g_direct3dDevice;
		extern ID3D11DeviceContext* g_direct3dImmediateContext;
		extern  eae6320::Platform::sDataFromFile* g_compiledVertexShader;
		extern ID3D11InputLayout* g_vertexLayout;

	}


}



#endif // ! _DIRECTX_REFRENCES_H

// Header Files
//=============

#include "../Graphics.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "Includes.h"
#include <string>
#include <sstream>
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../../Platform/Platform.h"
#include "../../Time/Time.h"
#include "../../Windows/Functions.h"
#include "../../Windows/OpenGl.h"
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#include "../GraphicsDataTypes.h"

// Static Data Initialization
//===========================

namespace
{
	// The is the main window handle from Windows
	HWND s_renderingWindow = NULL;
	// These are Windows-specific interfaces
	HDC s_deviceContext = NULL;
	HGLRC s_openGlRenderingContext = NULL;
	GLuint s_samplerId = 0;
	// This struct determines the layout of the geometric data that the CPU will send to the GPU


	// The vertex buffer holds the data for each vertex
	GLuint s_vertexArrayId = 0;
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
	// OpenGL debuggers don't seem to support freeing the vertex buffer
	// and letting the vertex array object hold a reference to it,
	// and so if debug info is enabled an explicit reference is held
	GLuint s_vertexBufferId = 0;
#endif




}

// Helper Function Declarations
//=============================

namespace
{

	bool CreateRenderingContext();
	bool setupSampler();

	// This helper struct exists to be able to dynamically allocate memory to get "log info"
	// which will automatically be freed when the struct goes out of scope
	struct sLogInfo
	{
		GLchar* memory;
		sLogInfo( const size_t i_size ) { memory = reinterpret_cast<GLchar*>( malloc( i_size ) ); }
		~sLogInfo() { if ( memory ) free( memory ); }
	};
}

// Interface
//==========

// Render



void eae6320::Graphics::BeginRenderFrame()
{
	
	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		// Black is usually used
		// Black is usually used
		glDepthMask(GL_TRUE);
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		// Clear depth to 1
		glClearDepth(1.0f);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		const GLbitfield clearColorAndDepth = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
		glClear(clearColorAndDepth);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	
	}

	// update the constant buffer
	{
		// update the struct (i.e. the memory that we own)
		s_FrameConstantBufferData.g_elapsedSecondCount_total = Time::GetElapsedSecondCount_total();
		s_FrameConstantBufferData.g_transform_cameraToScreen = m_currentCamera->getCameraToScreen();
		s_FrameConstantBufferData.g_transform_worldToCamera = m_currentCamera->getWorldToCamera();
		// make the uniform buffer active
		{
			s_frameConstantbuffer.bindBuffers();
		}
		// copy the updated memory to the gpu
	}
	s_frameConstantbuffer.UpdateData(&s_FrameConstantBufferData);
	

	


}


void eae6320::Graphics::EndRenderFrame()
{
	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it, the contents of the back buffer must be swapped with the "front buffer"
	// (which is what the user sees)
	{
		BOOL result = SwapBuffers(s_deviceContext);
		EAE6320_ASSERT(result != FALSE);
	}
}




// Initialization / Clean Up
//==========================

bool eae6320::Graphics::Initialize( const sInitializationParameters& i_initializationParameters )
{
	std::string errorMessage;

	s_renderingWindow = i_initializationParameters.mainWindow;

	// Load any required OpenGL extensions
	if ( !OpenGlExtensions::Load( &errorMessage ) )
	{
		EAE6320_ASSERTF( false, errorMessage.c_str() );
		Logging::OutputError( errorMessage.c_str() );
		return false;
	}
	// Create an OpenGL rendering context
	if ( !CreateRenderingContext() )
	{
		EAE6320_ASSERT( false );
		return false;
	}
	setupSampler();
	glEnable(GL_CULL_FACE);
	// Depth Testing
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	// Depth Writing

	const GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		
		EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
		Logging::OutputError("OpenGL failed to enable culling: %s",
			reinterpret_cast<const char*>(gluErrorString(errorCode)));
	}

	
	if ( !s_frameConstantbuffer.initialize(ConstantBufferTypes::FrameData,sizeof(s_FrameConstantBufferData),&s_FrameConstantBufferData) )
	{
		EAE6320_ASSERT( false );
		return false;
	}
	
	if (!s_drawCallConstantbuffer.initialize(ConstantBufferTypes::DrawCallData, sizeof(sDrawCall),nullptr))
	{
		EAE6320_ASSERT(false);
		return false;
	}
	{
		s_frameConstantbuffer.bindBuffers();
		s_drawCallConstantbuffer.bindBuffers();
	}
	
	s_listOfDrawData.clear();
	return true;
}

bool eae6320::Graphics::CleanUp()
{
	bool wereThereErrors = false;

	if (s_openGlRenderingContext != NULL)
	{
		
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
		if (s_vertexBufferId != 0)
		{
			const GLsizei bufferCount = 1;
			glDeleteBuffers(bufferCount, &s_vertexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			s_vertexBufferId = 0;
		}
#endif
		if (s_vertexArrayId != 0)
		{
			const GLsizei arrayCount = 1;
			glDeleteVertexArrays(arrayCount, &s_vertexArrayId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to delete the vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			s_vertexArrayId = 0;
		}
		if (s_samplerId != 0)
		{
			glDeleteSamplers(1,&s_samplerId);
		}

		s_frameConstantbuffer.cleanUp();
		s_drawCallConstantbuffer.cleanUp();

		if (wglMakeCurrent(s_deviceContext, NULL) != FALSE)
		{
			if (wglDeleteContext(s_openGlRenderingContext) == FALSE)
			{
				wereThereErrors = true;
				const std::string windowsErrorMessage = Windows::GetLastSystemError();
				EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
				Logging::OutputError("Windows failed to delete the OpenGL rendering context: %s", windowsErrorMessage.c_str());
			}
		}
		else
		{
			wereThereErrors = true;
			const std::string windowsErrorMessage = Windows::GetLastSystemError();
			EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
			Logging::OutputError("Windows failed to unset the current OpenGL rendering context: %s", windowsErrorMessage.c_str());
		}
		s_openGlRenderingContext = NULL;
	}

	if (s_deviceContext != NULL)
	{
		// The documentation says that this call isn't necessary when CS_OWNDC is used
		ReleaseDC(s_renderingWindow, s_deviceContext);
		s_deviceContext = NULL;
	}


	s_renderingWindow = NULL;

	return !wereThereErrors;
}

// Helper Function Declarations
//=============================

namespace
{
	

	bool CreateRenderingContext()
	{
		// Get the device context
		{
			s_deviceContext = GetDC(s_renderingWindow);
			if (s_deviceContext == NULL)
			{
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Windows failed to get the device context");
				return false;
			}
		}
		// Set the pixel format for the window
		// (This can only be done _once_ for a given window)
		{
			// Get the ID of the desired pixel format
			int pixelFormatId;
			{
				// Create a key/value list of attributes that the pixel format should have
				const int desiredAttributes[] =
				{
					WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
					WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
					WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
					WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
					WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
					WGL_COLOR_BITS_ARB, 24,
					WGL_RED_BITS_ARB, 8,
					WGL_GREEN_BITS_ARB, 8,
					WGL_BLUE_BITS_ARB, 8,
					WGL_DEPTH_BITS_ARB, 24,
					WGL_STENCIL_BITS_ARB, 8,
					// NULL terminator
					NULL
				};
				const float* const noFloatAttributes = NULL;
				const unsigned int onlyReturnBestMatch = 1;
				unsigned int returnedFormatCount;
				if (wglChoosePixelFormatARB(s_deviceContext, desiredAttributes, noFloatAttributes, onlyReturnBestMatch,
					&pixelFormatId, &returnedFormatCount) != FALSE)
				{
					if (returnedFormatCount == 0)
					{
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Windows couldn't find a pixel format that satisfied the desired attributes");
						return false;
					}
				}
				else
				{
					const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
					EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
					eae6320::Logging::OutputError("Windows failed to choose the closest pixel format: %s", windowsErrorMessage.c_str());
					return false;
				}
			}
			// Set it
			{
				PIXELFORMATDESCRIPTOR pixelFormatDescriptor = { 0 };
				{
					// I think that the values of this struct are ignored
					// and unnecessary when using wglChoosePixelFormatARB() instead of ChoosePixelFormat(),
					// but the documentation is very unclear and so filling it in seems the safest bet
					pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
					pixelFormatDescriptor.nVersion = 1;
					pixelFormatDescriptor.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
					pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
					pixelFormatDescriptor.cColorBits = 24;
					pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
					pixelFormatDescriptor.cDepthBits = 24;
					pixelFormatDescriptor.cStencilBits = 8;
				}
				if (SetPixelFormat(s_deviceContext, pixelFormatId, &pixelFormatDescriptor) == FALSE)
				{
					const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
					EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
					eae6320::Logging::OutputError("Windows couldn't set the desired pixel format: %s", windowsErrorMessage.c_str());
					return false;
				}
			}
		}
		// Create an OpenGL rendering context and make it current
		{
			// Create the context
			{
				// Create a key/value list of attributes that the context should have
				const int desiredAttributes[] =
				{
					// Request at least version 4.2
					WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
					WGL_CONTEXT_MINOR_VERSION_ARB, 2,
					// Request only "core" functionality and not "compatibility"
					// (i.e. only use modern features of version 4.2)
					WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
					WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
					// NULL terminator
					NULL
				};
				const HGLRC noSharedContexts = NULL;
				s_openGlRenderingContext = wglCreateContextAttribsARB(s_deviceContext, noSharedContexts, desiredAttributes);
				if (s_openGlRenderingContext == NULL)
				{
					DWORD errorCode;
					const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError(&errorCode);
					std::ostringstream errorMessage;
					errorMessage << "Windows failed to create an OpenGL rendering context: ";
					if ((errorCode == ERROR_INVALID_VERSION_ARB)
						|| (HRESULT_CODE(errorCode) == ERROR_INVALID_VERSION_ARB))
					{
						errorMessage << "The requested version number is invalid";
					}
					else if ((errorCode == ERROR_INVALID_PROFILE_ARB)
						|| (HRESULT_CODE(errorCode) == ERROR_INVALID_PROFILE_ARB))
					{
						errorMessage << "The requested profile is invalid";
					}
					else
					{
						errorMessage << windowsErrorMessage;
					}
					EAE6320_ASSERTF(false, errorMessage.str().c_str());
					eae6320::Logging::OutputError(errorMessage.str().c_str());

					return false;
				}
			}
			// Set it as the rendering context of this thread
			if (wglMakeCurrent(s_deviceContext, s_openGlRenderingContext) == FALSE)
			{
				const std::string windowsErrorMessage = eae6320::Windows::GetLastSystemError();
				EAE6320_ASSERTF(false, windowsErrorMessage.c_str());
				eae6320::Logging::OutputError("Windows failed to set the current OpenGL rendering context: %s",
					windowsErrorMessage.c_str());
				return false;
			}
		}

		return true;
	}
	bool setupSampler()
	{
		{
			const GLsizei samplerStateCount = 1;
			glGenSamplers(samplerStateCount, &s_samplerId);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				if (s_samplerId != 0)
				{
					// Linear Filtering
					glSamplerParameteri(s_samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
					glSamplerParameteri(s_samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
					// If UVs go outside [0,1] wrap them around (so that textures can tile)
					glSamplerParameteri(s_samplerId, GL_TEXTURE_WRAP_S, GL_REPEAT);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
					glSamplerParameteri(s_samplerId, GL_TEXTURE_WRAP_T, GL_REPEAT);
					EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
				}
				else
				{
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("OpenGL failed to create a sampler state");
					return false;
				}
			}
			else
			{
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to create a sampler state: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return false;
			}
		}
		// Bind the sampler state
		{
			// We will never be required to use more than one texture in an Effect in this class,
			// but it doesn't hurt to bind the state to a few extra texture units
			// just in case you decide to try using more
			const GLuint maxTextureUnitCountYouThinkYoullUse = 5;
			for (GLuint i = 0; i < maxTextureUnitCountYouThinkYoullUse; ++i)
			{
				glBindSampler(i, s_samplerId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to bind the sampler state to texture unit %u: %s",
						i, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return false;
				}
			}
		}
		return true;
	}



}

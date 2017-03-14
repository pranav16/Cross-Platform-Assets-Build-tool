#include  "../Effects.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
#include "../../Platform/Platform.h"
#include "../GraphicsDataTypes.h"
#include "../cRenderState.h"
#include "Includes.h"
namespace eae6320
{
	namespace Graphics
	{
		struct sEffectsData
		{
			GLuint programId;
			cRenderState renderState;
		};

	}
}
struct sLogInfo
{
	GLchar* memory;
	sLogInfo(const size_t i_size) { memory = reinterpret_cast<GLchar*>(malloc(i_size)); }
	~sLogInfo() { if (memory) free(memory); }
};

bool  eae6320::Graphics::Effects::load(const char* effectsPath)
{
	bool whereThereErrors = false;
	m_effectsData = new sEffectsData;
	std::string errorMsg;
	Platform::sDataFromFile dataFromFile;
	whereThereErrors = Platform::LoadBinaryFile(effectsPath, dataFromFile, &errorMsg);
	whereThereErrors = !whereThereErrors;
	if (whereThereErrors)
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
	// Create a program
	{
		m_effectsData->programId = glCreateProgram();
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to create a program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return false;
		}
		else if (m_effectsData->programId == 0)
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("OpenGL failed to create a program");
			return false;
		}
	}
	if (!LoadVertexShader(_vertexShaderPath))
	{
		EAE6320_ASSERT(false);
		return false;
	}
	if (!LoadFragmentShader(fragmentShader))
	{
		EAE6320_ASSERT(false);
		return false;
	}

	{
		glLinkProgram(m_effectsData->programId);
		GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get link info
			// (this won't be used unless linking fails
			// but it can be useful to look at when debugging)
			std::string linkInfo;
			{
				GLint infoSize;
				glGetProgramiv(m_effectsData->programId, GL_INFO_LOG_LENGTH, &infoSize);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					sLogInfo info(static_cast<size_t>(infoSize));
					GLsizei* dontReturnLength = NULL;
					glGetProgramInfoLog(m_effectsData->programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						linkInfo = info.memory;
					}
					else
					{
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to get link info of the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						return false;
					}
				}
				else
				{
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get the length of the program link info: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return false;
				}
			}
			// Check to see if there were link errors
			GLint didLinkingSucceed;
			{
				glGetProgramiv(m_effectsData->programId, GL_LINK_STATUS, &didLinkingSucceed);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didLinkingSucceed == GL_FALSE)
					{
						EAE6320_ASSERTF(false, linkInfo.c_str());
						eae6320::Logging::OutputError("The program failed to link: %s",
							linkInfo.c_str());
						return false;
					}
				}
				else
				{
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to find out if linking of the program succeeded: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return false;
				}
			}
		}
		else
		{
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to link the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return false;
		}
	}

	return true && m_effectsData->renderState.Initialize(*renderStates);
	
}
bool eae6320::Graphics::Effects::cleanUp()
{
	bool whereThereErrors = false;
	if (m_effectsData)
	{
		if (m_effectsData->programId != 0)
		{
			glDeleteProgram(m_effectsData->programId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				whereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				Logging::OutputError("OpenGL failed to delete the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
			}
			m_effectsData->programId = 0;
		}
		delete m_effectsData;
		m_effectsData = nullptr;
	}

	return !whereThereErrors;
}
bool eae6320::Graphics::Effects::bind()
{
	{
		glUseProgram(m_effectsData->programId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
		m_effectsData->renderState.Bind();
	}
	return true;
}


bool eae6320::Graphics::Effects::LoadFragmentShader(const char* fragmentShaderPath)
{
	{
		GLboolean isShaderCompilingSupported;
		glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
		if (!isShaderCompilingSupported)
		{
			//eae6320::UserOutput::Print( "Compiling shaders at run-time isn't supported on this implementation (this should never happen)" );
			return false;
		}
	}

	bool wereThereErrors = false;

	// Load the source code from file and set it into a shader
	GLuint fragmentShaderId = 0;
	eae6320::Platform::sDataFromFile dataFromFile;
	{
		// Load the shader source code
		{
			const char* path_sourceCode = fragmentShaderPath;
			std::string errorMessage;
			if (!eae6320::Platform::LoadBinaryFile(path_sourceCode, dataFromFile, &errorMessage))
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, errorMessage.c_str());
				eae6320::Logging::OutputError("Failed to load the fragment shader \"%s\": %s",
					path_sourceCode, errorMessage.c_str());
				goto OnExit;
			}
		}
		// Generate a shader
		fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		{
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to get an unused fragment shader ID: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
			else if (fragmentShaderId == 0)
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("OpenGL failed to get an unused fragment shader ID");
				goto OnExit;
			}
		}
		// Set the source code into the shader
		{
			const GLsizei shaderSourceCount = 1;
			const GLint length = static_cast<GLuint>(dataFromFile.size);
			glShaderSource(fragmentShaderId, shaderSourceCount, reinterpret_cast<GLchar**>(&dataFromFile.data), &length);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the fragment shader source code: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
	}
	// Compile the shader source code
	{
		glCompileShader(fragmentShaderId);
		GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get compilation info
			// (this won't be used unless compilation fails
			// but it can be useful to look at when debugging)
			std::string compilationInfo;
			{
				GLint infoSize;
				glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoSize);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					sLogInfo info(static_cast<size_t>(infoSize));
					GLsizei* dontReturnLength = NULL;
					glGetShaderInfoLog(fragmentShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						compilationInfo = info.memory;
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to get compilation info about the fragment shader source code: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get the length of the fragment shader compilation info: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			// Check to see if there were compilation errors
			GLint didCompilationSucceed;
			{
				glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didCompilationSucceed == GL_FALSE)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, compilationInfo.c_str());
						eae6320::Logging::OutputError("OpenGL failed to compile the fragment shader: %s",
							compilationInfo.c_str());
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to find if compilation of the fragment shader source code succeeded: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to compile the fragment shader source code: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	// Attach the shader to the program
	{
		glAttachShader(m_effectsData->programId, fragmentShaderId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to attach the fragment to the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

OnExit:

	if (fragmentShaderId != 0)
	{
		// Even if the shader was successfully compiled
		// once it has been attached to the program we can (and should) delete our reference to it
		// (any associated memory that OpenGL has allocated internally will be freed
		// once the program is deleted)
		glDeleteShader(fragmentShaderId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the fragment shader ID %u: %s",
				fragmentShaderId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		fragmentShaderId = 0;
	}
	dataFromFile.Free();

	return !wereThereErrors;
}


bool eae6320::Graphics::Effects::LoadVertexShader(const char* vertexShaderPath)
{
	// Verify that compiling shaders at run-time is supported
	{
		GLboolean isShaderCompilingSupported;
		glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
		if (!isShaderCompilingSupported)
		{
			//eae6320::UserOutput::Print( "Compiling shaders at run-time isn't supported on this implementation (this should never happen)" );
			return false;
		}
	}

	bool wereThereErrors = false;

	// Load the source code from file and set it into a shader
	GLuint vertexShaderId = 0;
	eae6320::Platform::sDataFromFile dataFromFile;
	{
		// Load the shader source code
		{
			const char* path_sourceCode = vertexShaderPath;
			std::string errorMessage;
			if (!eae6320::Platform::LoadBinaryFile(path_sourceCode, dataFromFile, &errorMessage))
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, errorMessage.c_str());
				eae6320::Logging::OutputError("Failed to load the vertex shader \"%s\": %s",
					path_sourceCode, errorMessage.c_str());
				goto OnExit;
			}
		}
		// Generate a shader
		vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		{
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to get an unused vertex shader ID: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
			else if (vertexShaderId == 0)
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("OpenGL failed to get an unused vertex shader ID");
				goto OnExit;
			}
		}
		// Set the source code into the shader
		{
			const GLsizei shaderSourceCount = 1;
			const GLint length = static_cast<GLuint>(dataFromFile.size);
			glShaderSource(vertexShaderId, shaderSourceCount, reinterpret_cast<GLchar**>(&dataFromFile.data), &length);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the vertex shader source code: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
	}
	// Compile the shader source code
	{
		glCompileShader(vertexShaderId);
		GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get compilation info
			// (this won't be used unless compilation fails
			// but it can be useful to look at when debugging)
			std::string compilationInfo;
			{
				GLint infoSize;
				glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoSize);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					sLogInfo info(static_cast<size_t>(infoSize));
					GLsizei* dontReturnLength = NULL;
					glGetShaderInfoLog(vertexShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						compilationInfo = info.memory;
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to get compilation info about the vertex shader source code: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get the length of the vertex shader compilation info: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			// Check to see if there were compilation errors
			GLint didCompilationSucceed;
			{
				glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
				errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didCompilationSucceed == GL_FALSE)
					{
						wereThereErrors = true;
						EAE6320_ASSERTF(false, compilationInfo.c_str());
						eae6320::Logging::OutputError("OpenGL failed to compile the vertex shader: %s",
							compilationInfo.c_str());
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to find if compilation of the vertex shader source code succeeded: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to compile the vertex shader source code: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	// Attach the shader to the program
	{
		glAttachShader(m_effectsData->programId, vertexShaderId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to attach the vertex to the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

OnExit:

	if (vertexShaderId != 0)
	{
		// Even if the shader was successfully compiled
		// once it has been attached to the program we can (and should) delete our reference to it
		// (any associated memory that OpenGL has allocated internally will be freed
		// once the program is deleted)
		glDeleteShader(vertexShaderId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the vertex shader ID %u: %s",
				vertexShaderId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		vertexShaderId = 0;
	}
	dataFromFile.Free();

	return !wereThereErrors;
}

bool eae6320::Graphics::Effects::createInputLayout(eae6320::Platform::sDataFromFile* compiledShader)
{
	//not need for opengl
	return true;
}

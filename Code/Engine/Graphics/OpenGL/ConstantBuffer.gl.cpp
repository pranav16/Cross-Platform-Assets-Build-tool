#include "Includes.h"
#include "../ConstantBuffer.h"
#include "../../Asserts/Asserts.h"
#include "../../Logging/Logging.h"
namespace eae6320
{
	namespace Graphics
	{
		struct ConstantBufferPlatformData
		{
			GLuint constantBufferId;

		};
	}

}

bool eae6320::Graphics::ConstantBuffer::initialize(const ConstantBufferTypes type,const size_t sizeOfBuffer,const void* payload)
{

	m_type = type;
	m_size = sizeOfBuffer;
	bool wereThereErrors = false;
	m_platfromData = new ConstantBufferPlatformData;
	// Create a uniform buffer object and make it active
	{
		const GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &m_platfromData->constantBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, m_platfromData->constantBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the new uniform buffer %u: %s",
					m_platfromData->constantBufferId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused uniform buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	// Allocate space and copy the constant data into the uniform buffer

		
		const GLenum usage = GL_DYNAMIC_DRAW;	// The buffer will be modified frequently and used to draw
		glBufferData(GL_UNIFORM_BUFFER, m_size, reinterpret_cast<const GLvoid*>(payload), usage);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the new uniform buffer %u: %s",
				m_platfromData->constantBufferId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}


OnExit:

	return !wereThereErrors;
}

bool eae6320::Graphics::ConstantBuffer::bindBuffers()
{
	
	// bind the constant buffer to the shader
	{
		const GLuint bindingpointassignedinshader = m_type;
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingpointassignedinshader, m_platfromData->constantBufferId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	return true;
}
bool eae6320::Graphics::ConstantBuffer::UpdateData(const void* payLoad)
{
	// make the uniform buffer active
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_platfromData->constantBufferId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	//// copy the updated memory to the gpu
	{
		GLintptr updateatthebeginning = 0;
		GLsizeiptr updatetheentirebuffer = static_cast<GLsizeiptr>(m_size);
		glBufferSubData(GL_UNIFORM_BUFFER, updateatthebeginning, updatetheentirebuffer, reinterpret_cast<const GLvoid*>(payLoad));
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	return true;
}

bool eae6320::Graphics::ConstantBuffer::cleanUp()
{
	bool wereThereErrors = false;
	if (m_platfromData->constantBufferId != 0)
	{
		const GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &m_platfromData->constantBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			Logging::OutputError("OpenGL failed to delete the constant buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		m_platfromData->constantBufferId = 0;
	}

	delete m_platfromData;
	m_size = 0;
	return !wereThereErrors;
}
#include "../Mesh.h"
#include "../../Logging/Logging.h"
#include "Includes.h"
#include "../../Asserts/Asserts.h"
#include "../Configuration.h"

namespace eae6320
{

	namespace Graphics
	{
		struct GraphicsPlatformData
		{
			GLuint s_vertexArrayId;
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
			// OpenGL debuggers don't seem to support freeing the vertex buffer
			// and letting the vertex array object hold a reference to it,
			// and so if debug info is enabled an explicit reference is held
			GLuint s_vertexBufferId = 0;
			GLuint s_indexBufferId = 0;
#endif

		};
	}
}
bool  eae6320::Graphics::Mesh::initialize(uint16_t vertexCount, sVertex* listOfVertcies, uint16_t indexCount, sIndex* listofIndicies)
{
	//EAE6320_ASSERT(m_listOfVerticies == nullptr);
	bool status;
	m_vertexCount = vertexCount;
	m_listOfVerticies = listOfVertcies;
	m_indexCount = indexCount;
	m_listOfIndicies = listofIndicies;
	for (int i = 0; i < indexCount; i++)
	{
	   uint16_t ind =	listofIndicies[i].index1;
	   uint16_t in =	    listofIndicies[i].index2;
		uint16_t jp =       listofIndicies[i].index3;
	}

	status = intializeVertexBuffer();
	
	return status;
}

bool eae6320::Graphics::Mesh::intializeVertexBuffer()
{
	bool wereThereErrors = false;
	GLuint vertexBufferId = 0;
	GLuint indexBufferId = 0;
	m_platformData = new  GraphicsPlatformData;
	// Create a vertex array object and make it active
	{
		const GLsizei arrayCount = 1;
		glGenVertexArrays(arrayCount, &(m_platformData->s_vertexArrayId));
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindVertexArray(m_platformData->s_vertexArrayId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the vertex array: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	// Create a vertex buffer object and make it active
	{
		const GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &vertexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	// Create a index buffer object and make it active
	{
		const GLsizei bufferCount = 1;
		glGenBuffers(bufferCount, &indexBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the vertex buffer: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	// Assign the data to the buffer
	{
		// Eventually the vertex data should come from a file but for now it is hard-coded here.
		// You will have to update this in a future assignment
		// (one of the most common mistakes in the class is to leave hard-coded values here).


		const unsigned int vertexCount = m_vertexCount;

		const unsigned int bufferSize = vertexCount * sizeof(eae6320::Graphics::sVertex);

		glBufferData(GL_ARRAY_BUFFER, bufferSize, reinterpret_cast<GLvoid*>(m_listOfVerticies),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	// Assign the data to the index buffer
	{
		// Eventually the vertex data should come from a file but for now it is hard-coded here.
		// You will have to update this in a future assignment
		// (one of the most common mistakes in the class is to leave hard-coded values here).


		const unsigned int indexCount = m_indexCount;

		const unsigned int bufferSize = indexCount * sizeof(eae6320::Graphics::sIndex);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, reinterpret_cast<GLvoid*>(m_listOfIndicies),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	// Initialize the vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		const GLsizei stride = sizeof(eae6320::Graphics::sVertex);

		// Position (0)
		// 3 floats == 12 bytes
		// Offset = 0
		{
			const GLuint vertexElementLocation = 0;
			const GLint elementCount = 3;
			const GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::sVertex, x)));
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		{
			const GLuint vertexElementLocation = 1;
			const GLint elementCount = 4;
			const GLboolean Normalized = GL_TRUE;	
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, Normalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::sVertex, r)));
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		{
			const GLuint vertexElementLocation = 2;
			const GLint elementCount = 2;
			const GLboolean Normalized = GL_TRUE;
			glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, Normalized, stride,
				reinterpret_cast<GLvoid*>(offsetof(eae6320::Graphics::sVertex, u)));
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(vertexElementLocation);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
	}

OnExit:

	if (m_platformData->s_vertexArrayId != 0)
	{
		// Unbind the vertex array
		// (this must be done before deleting the vertex buffer)
		glBindVertexArray(0);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// The vertex and index buffer objects can be freed
			// (the vertex array object will still hold references to them,
			// and so they won't actually goes away until it gets freed).
			// Unfortunately debuggers don't work well when these are freed
			// (gDEBugger just doesn't show anything and RenderDoc crashes),
			// and so don't free them if debug info is enabled.
			if (vertexBufferId != 0)
			{

#ifndef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &vertexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to vertex buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
				vertexBufferId = 0;
#else
				m_platformData->s_vertexBufferId = vertexBufferId;
#endif
			}
			if (indexBufferId != 0)
			{

#ifndef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &indexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to vertex buffer: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					goto OnExit;
				}
				indexBufferId = 0;
#else
				m_platformData->s_indexBufferId = indexBufferId;
#endif
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to unbind the vertex array: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

	return !wereThereErrors;
	return true;
}

void eae6320::Graphics::Mesh::draw()
{	
	
	//eae6320::Graphics::RenderFrame(m_platformData, m_vertexCount);
	// The mode defines how to interpret multiple vertices as a single "primitive";
	// we define a triangle list
	// (meaning that every primitive is a triangle and will be defined by three vertices)
	// Bind a specific vertex buffer to the device as a data source
	{
		glBindVertexArray(m_platformData->s_vertexArrayId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}

	// Render triangles from the currently-bound buffers
	{
		// The mode defines how to interpret multiple vertices as a single "primitive";
		// we define a triangle list
		// (meaning that every primitive is a triangle and will be defined by three vertices)
		const GLenum mode = GL_TRIANGLES;
		// Every index is a 16 bit unsigned integer
		const GLenum indexType = GL_UNSIGNED_SHORT;
		// It's possible to start rendering primitives in the middle of the stream
		const GLvoid* const offset = 0;
		const unsigned int indexCount =  m_indexCount * 3;
		glDrawElements(mode,indexCount, indexType, offset);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}

}

bool eae6320::Graphics::Mesh::cleanUp()
{
bool whereThereErrors = false;
#ifdef EAE6320_GRAPHICS_ISDEVICEDEBUGINFOENABLED
if (m_platformData->s_vertexBufferId != 0)
{
	const GLsizei bufferCount = 1;
	glDeleteBuffers(bufferCount, &m_platformData->s_vertexBufferId);
	const GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		whereThereErrors = true;
		EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
		Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
			reinterpret_cast<const char*>(gluErrorString(errorCode)));
	}
	m_platformData->s_vertexBufferId = 0;
}
if (m_platformData->s_indexBufferId != 0)
{
	const GLsizei bufferCount = 1;
	glDeleteBuffers(bufferCount, &m_platformData->s_indexBufferId);
	const GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		whereThereErrors = true;
		EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
		Logging::OutputError("OpenGL failed to delete the vertex buffer: %s",
			reinterpret_cast<const char*>(gluErrorString(errorCode)));
	}
	m_platformData->s_indexBufferId = 0;
}



#endif
if (m_platformData->s_vertexArrayId != 0)
{
	const GLsizei arrayCount = 1;
	glDeleteVertexArrays(arrayCount, &m_platformData->s_vertexArrayId);
	const GLenum errorCode = glGetError();
	if (errorCode != GL_NO_ERROR)
	{
		whereThereErrors = true;
		EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
		Logging::OutputError("OpenGL failed to delete the vertex array: %s",
			reinterpret_cast<const char*>(gluErrorString(errorCode)));
	}
	m_platformData->s_vertexArrayId = 0;
}


delete m_platformData;
delete[] m_listOfVerticies;
m_platformData = nullptr;
m_listOfVerticies = nullptr;
m_vertexCount = 0;
return !whereThereErrors;
}
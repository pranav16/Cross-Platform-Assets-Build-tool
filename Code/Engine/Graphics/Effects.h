#pragma once
#ifndef _EFFECTS_H_
#define _EFFECTS_H_

namespace eae6320
{
	namespace Platform
	{
		struct sDataFromFile;
	}
}
namespace eae6320
{
	namespace Graphics
	{
		struct sEffectsData;
		class Effects
		{
		public:
		
			bool load(const char* effectsPath);
			bool cleanUp();
			bool bind();

		private:
			bool LoadVertexShader(const char* vertexShaderPath);
			bool LoadFragmentShader(const char* fragmentShaderPath);
			bool createInputLayout(eae6320::Platform::sDataFromFile* data);
			sEffectsData* m_effectsData;
		};
	}
}

#endif // !_EFFECTS_H_

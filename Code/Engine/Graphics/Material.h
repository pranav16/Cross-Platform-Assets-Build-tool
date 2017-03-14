#pragma once
#ifndef _MATERIAL_H_
#define _MATERIAL_H_
namespace eae6320
{

	namespace Graphics
	{


		class  Material
		{
		public:
			bool load(const char* i_path);
			bool bind();
			bool cleanUp();
			float getTextureWidth();
			float getTextureHeight();

		private:
			class Effects* m_effect;
			class ConstantBuffer* m_constantBuffer;
			class cTexture* m_texture;
		};

	}
}


#endif

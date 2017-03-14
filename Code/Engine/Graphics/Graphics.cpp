// Header Files
//=============

#include "Graphics.h"
#include "GraphicsDataTypes.h"

namespace eae6320
{
	namespace Graphics
	{
		ConstantBuffer s_frameConstantbuffer;
		ConstantBuffer s_drawCallConstantbuffer;
		std::vector<sDrawData> s_listOfDrawData;
		std::vector<sSpriteDrawData> s_listOfSpriteDrawData;
		sConstantBufferFrameData s_FrameConstantBufferData;
		Camera* m_currentCamera;
	
	}
}
bool eae6320::Graphics::setCurrentCamera(Camera* i_camera)
{

	m_currentCamera = i_camera;
	return true;
}

void eae6320::Graphics::SubmitDrawData(std::vector<RenderData*>i_listOfRenderables, std::vector<SpriteRenderData*>i_listOfSprites)
{
	s_listOfDrawData.clear();
	for (unsigned int i = 0; i < i_listOfRenderables.size();i++)
	{
		sDrawData data;
		data.mesh = i_listOfRenderables[i]->mesh;
		data.material = i_listOfRenderables[i]->material;
		data.s_drawBuffer.g_transform_localToWorld = Math::cMatrix_transformation(i_listOfRenderables[i]->rotation, i_listOfRenderables[i]->position);
		s_listOfDrawData.push_back(data);
	}
	s_listOfSpriteDrawData.clear();
	for (unsigned int i = 0; i < i_listOfSprites.size(); i++)
	{
		sSpriteDrawData data;
		data.sprite = i_listOfSprites[i]->sprite;
		data.material = i_listOfSprites[i]->material;
		s_listOfSpriteDrawData.push_back(data);
	}

}
void eae6320::Graphics::RenderFrame()
{
	for (unsigned int i = 0; i < s_listOfDrawData.size(); i++)
	{
		// update the struct (i.e. the memory that we own)
		{
			// make the uniform buffer active
			// copy the updated memory to the gpu
			s_listOfDrawData[i].material->bind();
			s_drawCallConstantbuffer.UpdateData(&s_listOfDrawData[i].s_drawBuffer);
		}
		s_listOfDrawData[i].mesh->draw();	
	}
	s_listOfDrawData.clear();

	for (unsigned int i = 0; i < s_listOfSpriteDrawData.size(); i++)
	{
		// update the struct (i.e. the memory that we own)
		{
			// make the uniform buffer active
			// copy the updated memory to the gpu
			s_listOfSpriteDrawData[i].material->bind();
			
		}
		s_listOfSpriteDrawData[i].sprite->Draw();
	}
	s_listOfSpriteDrawData.clear();
}
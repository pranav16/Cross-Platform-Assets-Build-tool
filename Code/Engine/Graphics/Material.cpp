#include "Material.h"
#include "ConstantBuffer.h"
#include "Effects.h"
#include "GraphicsDataTypes.h"
#include "../Platform/Platform.h"
#include"../Logging/Logging.h"
#include "cTexture.h"


bool  eae6320::Graphics::Material::load(const char* i_path)
{
	bool whereThereErrors = false;
	m_effect = new Effects();
	m_constantBuffer = new ConstantBuffer();
	m_texture = new cTexture();
	Platform::sDataFromFile dataFromFile;
	std::string errorMsg;
	whereThereErrors = Platform::LoadBinaryFile(i_path, dataFromFile, &errorMsg);
	whereThereErrors = !whereThereErrors;
	if (whereThereErrors)
	{
		Logging::OutputError(errorMsg.c_str());
		return whereThereErrors;
	}
	uint8_t* data = reinterpret_cast<uint8_t*>(dataFromFile.data);
	sMaterialConstants* constants = reinterpret_cast<sMaterialConstants*>(data);
	data += sizeof(sMaterialConstants);
	uint16_t sizeOfEffect = *reinterpret_cast<uint16_t*>(data);
	data += sizeof(uint16_t);
	const char* effectPath = reinterpret_cast<const char*>(data);
	data += sizeOfEffect;
	const char* texturePath = reinterpret_cast<const char*>(data);
	whereThereErrors = m_effect->load(effectPath) && m_constantBuffer->initialize(eae6320::Graphics::MaterialData, sizeof(sMaterialConstants), constants) && m_texture->Load(texturePath);
	m_constantBuffer->UpdateData(constants);
	dataFromFile.Free();
	return whereThereErrors;
}
float eae6320::Graphics::Material::getTextureWidth()
{
	return m_texture->GetWidth();
}
float eae6320::Graphics::Material::getTextureHeight()
{
	return m_texture->GetHeight();
}
bool eae6320::Graphics::Material::bind()
{
	m_texture->Bind(0);
	return m_effect->bind() && m_constantBuffer->bindBuffers();
	
}
bool eae6320::Graphics::Material::cleanUp()
{
	m_constantBuffer->cleanUp();
	m_effect->cleanUp();
	m_texture->CleanUp();
	delete m_constantBuffer;
	delete m_effect;
	delete m_texture;
	m_constantBuffer = nullptr;
	m_effect = nullptr;
	m_texture = nullptr;
	
	return true;
}
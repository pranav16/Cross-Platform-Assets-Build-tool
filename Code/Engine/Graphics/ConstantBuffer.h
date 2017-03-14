#pragma once
#ifndef  _CONSTANT_BUFFER_H

namespace eae6320
{
	namespace Graphics
	{
		struct ConstantBufferPlatformData;
		enum ConstantBufferTypes
		{
			FrameData = 0, DrawCallData = 1, MaterialData = 2
		};
		class ConstantBuffer
		{
		public:
			ConstantBuffer();
			~ConstantBuffer();
			bool initialize(const ConstantBufferTypes type, const size_t sizeOfBuffer,const void* payload);
			bool bindBuffers();
			bool UpdateData(const void* payLoad);
			bool cleanUp();

		private:
			ConstantBufferPlatformData* m_platfromData;
			ConstantBufferTypes m_type;
			size_t m_size;
		};
	} // namespace Graphics
}//namespace eae6320
#endif // ! _CONSTANT_BUFFER_H

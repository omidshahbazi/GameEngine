// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Pass.h>
#include <Rendering\CPUConstantBuffer.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace Rendering
	{
		using namespace Private;

#define IMPLEMENT_SET_TEXTURE(HashValue, Pointer) \
		TextureConstantInfo info; \
		info.Hash = HashValue; \
		info.Value = Pointer; \
		m_TextureInfo[info.Hash] = info; \
		return true;

#define CLEANUP_BUFFER_INFO() \
		for (auto& constant : m_BufferInfo) \
			RenderingAllocators::ContainersAllocator_Deallocate(constant.GetSecond().Value); \
		m_BufferInfo.Clear();

		Pass::Pass(void) :
			m_Program(nullptr),
			m_Queue(RenderQueues::Default)
		{
		}

		Pass::Pass(ProgramResource* Program) :
			m_Queue(RenderQueues::Default)
		{
			SetProgram(Program);
		}

		Pass::Pass(const Pass& Other)
		{
			*this = Other;
		}

		Pass::~Pass(void)
		{
			CLEANUP_BUFFER_INFO();
		}

		ConstantBuffer* Pass::GetConstantBuffer(ConstantHash Hash)
		{
			CPUConstantBuffer* buffer = nullptr;

			if (m_BufferInfo.Contains(Hash))
				buffer = ReinterpretCast(CPUConstantBuffer*, m_BufferInfo[Hash].Value);
			else
			{
				uint16 size = 1024;

				if (!m_Program->IsNull())
				{
					const StructMetaInfo* structInfo = (*m_Program)->GetStructInfoOf(Hash);
					if (structInfo != nullptr)
						size = structInfo->Size;
				}

				buffer = RenderingAllocators::ContainersAllocator_Allocate<CPUConstantBuffer>();
				Construct(buffer, size);

				BufferConstantInfo info;
				info.Hash = Hash;
				info.Value = buffer;
				m_BufferInfo[info.Hash] = info;
			}

			return buffer;
		}

		ConstantBuffer* Pass::GetConstantBuffer(const String& Name)
		{
			return GetConstantBuffer(GetHash(Name));
		}

		bool Pass::SetTexture(ConstantHash Hash, const TextureResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(Hash, ConstCast(TextureResource*, Value));
		}

		bool Pass::SetTexture(const String& Name, const TextureResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(GetHash(Name), ConstCast(TextureResource*, Value));
		}

		bool Pass::SetSprite(ConstantHash Hash, const SpriteResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(Hash, ReinterpretCast(TextureResource*, ConstCast(SpriteResource*, Value)));
		}

		bool Pass::SetSprite(const String& Name, const SpriteResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(GetHash(Name), ReinterpretCast(TextureResource*, ConstCast(SpriteResource*, Value)));
		}

		void Pass::SetProgram(ProgramResource* Program)
		{
			if (m_Program == Program)
				return;

			m_Program = Program;

			CLEANUP_BUFFER_INFO();

			m_TextureInfo.Clear();
		}

		void Pass::SetRenderState(const IDevice::State& State)
		{
			PlatformMemory::Copy(&State, &m_RenderState, 1);
		}

		Pass& Pass::operator=(const Pass& Other)
		{
			m_Program = Other.m_Program;
			m_Queue = Other.m_Queue;

			for (auto& bufferInfo : Other.m_BufferInfo)
			{
				BufferConstantInfo info;
				info.Hash = bufferInfo.GetFirst();
				info.Value = ReinterpretCast(CPUConstantBuffer*, bufferInfo.GetSecond().Value)->Clone();
				m_BufferInfo[info.Hash] = info;
			}

			m_TextureInfo = Other.m_TextureInfo;

			m_RenderState = Other.m_RenderState;

			return *this;
		}

#undef CLEANUP_BUFFER_INFO
#undef IMPLEMENT_SET_TEXTURE
	}
}
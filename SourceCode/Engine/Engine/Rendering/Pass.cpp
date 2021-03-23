// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Pass.h>
#include <Rendering\ConstantBuffer.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace Rendering
	{
		using namespace Private;

#define IMPLEMENT_SET_TEXTURE(HashValue, Pointer) \
		ConstantInfo info; \
		info.Hash = HashValue; \
		info.Value = Pointer; \
		m_ConstantInfos[HashValue] = info; \
		return true;

		class PassConstantBuffer : public ConstantBuffer
		{
		public:
			PassConstantBuffer(uint32 Size) :
				ConstantBuffer(nullptr, Size, 0)
			{
				m_Address = RenderingAllocators::ContainersAllocator_AllocateArray<byte>(Size);
			}

			void Lock(Access Access) override
			{
			}

			void Unlock(void) override
			{
			}

		protected:
			~PassConstantBuffer(void)
			{
				RenderingAllocators::ContainersAllocator_Deallocate(m_Address);
			}

			byte* GetBuffer(Access Access)
			{
				return m_Address;
			}

		private:
			byte* m_Address;
		};

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
			for (auto& constant : m_ConstantBuffers)
				RenderingAllocators::ContainersAllocator_Deallocate(constant);
		}

		ConstantBuffer* Pass::GetConstantBuffer(ConstantHash Hash)
		{
			PassConstantBuffer* buffer = nullptr;

			if (m_ConstantInfos.Contains(Hash))
				buffer = m_ConstantInfos[Hash].Value.Get<PassConstantBuffer*>();
			else
			{
				const StructMetaInfo* structInfo = (*m_Program)->GetStructInfoOf(Hash);
				if (structInfo == nullptr)
					return nullptr;

				buffer = RenderingAllocators::ContainersAllocator_Allocate<PassConstantBuffer>();
				Construct(buffer, structInfo->Size);

				ConstantInfo info;
				info.Hash = Hash;
				info.Value = buffer;
				m_ConstantInfos[Hash] = info;

				m_ConstantBuffers.Add(buffer);
			}

			return buffer;
		}

		ConstantBuffer* Pass::GetConstantBuffer(const String& Name)
		{
			return GetConstantBuffer(GetHash(Name));
		}

		bool Pass::SetTexture(ConstantHash Hash, const TextureResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(Hash, ReinterpretCast(void*, ConstCast(TextureResource*, Value)));
		}

		bool Pass::SetTexture(const String& Name, const TextureResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(GetHash(Name), ReinterpretCast(void*, ConstCast(TextureResource*, Value)));
		}

		bool Pass::SetSprite(ConstantHash Hash, const SpriteResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(Hash, ReinterpretCast(void*, ConstCast(SpriteResource*, Value)));
		}

		bool Pass::SetSprite(const String& Name, const SpriteResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(GetHash(Name), ReinterpretCast(void*, ConstCast(SpriteResource*, Value)));
		}

		void Pass::SetProgram(ProgramResource* Program)
		{
			if (m_Program == Program)
				return;

			m_Program = Program;

			m_ConstantInfos.Clear();
			m_ConstantBuffers.Clear();
		}

		void Pass::SetRenderState(const IDevice::State& State)
		{
			PlatformMemory::Copy(&State, &m_RenderState, 1);
		}

#undef IMPLEMENT_SET_TEXTURE
	}
}
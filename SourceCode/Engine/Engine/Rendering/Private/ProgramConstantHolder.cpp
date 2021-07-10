// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramConstantHolder.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\ConstantBuffer.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Hash.h>

namespace Engine
{
	using namespace Utility;

	namespace Rendering
	{
		namespace Private
		{
#define IMPLEMENT_SET_TEXTURE(Hash, Pointer) \
		if (!m_Textures.Contains(Hash)) \
			return false; \
		m_Textures[Hash].Value = Pointer; \
		return true;

			ProgramConstantHolder::~ProgramConstantHolder(void)
			{
				CleanupData();
			}

			ConstantBuffer* ProgramConstantHolder::GetConstantBuffer(ConstantHash Hash)
			{
				ConstantBuffer* buffer = nullptr;

				if (m_Buffers.Contains(Hash))
					return m_Buffers[Hash].Value;

				return nullptr;
			}

			ConstantBuffer* ProgramConstantHolder::GetConstantBuffer(const String& Name)
			{
				return GetConstantBuffer(GetHash(Name));
			}

			bool ProgramConstantHolder::SetBuffer(ConstantHash Hash, const byte* Data, uint16 Size)
			{
				ConstantBuffer* buffer = GetConstantBuffer(Hash);
				if (buffer == nullptr)
					return false;

				buffer->Set(Data, Size);

				return true;
			}

			ProgramConstantHolder::ConstantHash ProgramConstantHolder::GetHash(const String& Name)
			{
				return Hash::CRC32(Name.GetValue(), Name.GetLength());
			}

			void ProgramConstantHolder::CreateBufferData(ConstantHandle Handle, const String& Name, const String& UserDefinedType, uint16 Size)
			{
				ConstantBuffer* buffer = RenderingManager::GetInstance()->GetActiveDevice()->CreateConstantBuffer(Size);

				m_Buffers[GetHash(Name)] = BufferConstantData(Handle, Name, UserDefinedType, buffer);
			}

			void ProgramConstantHolder::CreateTextureData(ConstantHandle Handle, const String& Name)
			{
				m_Textures[GetHash(Name)] = TextureConstantData(Handle, Name, nullptr);
			}

			void ProgramConstantHolder::CloneData(const ProgramConstantHolder& Other)
			{
				for (auto& bufferInfo : Other.m_Buffers)
				{
					auto data = bufferInfo.GetSecond();

					const auto otherBuffer = data.Value;

					data.Value = RenderingManager::GetInstance()->GetActiveDevice()->CreateConstantBuffer(data.Value->GetSize());
					data.Value->Copy(*otherBuffer);

					m_Buffers[data.Hash] = data;
				}

				m_Textures = Other.m_Textures;
			}

			void ProgramConstantHolder::MoveData(ProgramConstantHolder&& Other)
			{
				for (auto& bufferInfo : Other.m_Buffers)
				{
					auto& data = bufferInfo.GetSecond();

					m_Buffers[data.Hash] = data;
				}

				Other.m_Buffers.Clear();

				m_Textures = Other.m_Textures;
			}

			void ProgramConstantHolder::CleanupData(void)
			{
				for (auto& constant : m_Buffers)
					RenderingManager::GetInstance()->GetActiveDevice()->DestroyConstantBuffer(constant.GetSecond().Value);

				m_Buffers.Clear();

				m_Textures.Clear();
			}

			bool ProgramConstantHolder::SetTexture(ConstantHash Hash, const TextureResource* Value)
			{
				IMPLEMENT_SET_TEXTURE(Hash, ConstCast(TextureResource*, Value));
			}

			bool ProgramConstantHolder::SetTexture(const String& Name, const TextureResource* Value)
			{
				IMPLEMENT_SET_TEXTURE(GetHash(Name), ConstCast(TextureResource*, Value));
			}

			bool ProgramConstantHolder::SetSprite(ConstantHash Hash, const SpriteResource* Value)
			{
				IMPLEMENT_SET_TEXTURE(Hash, ReinterpretCast(TextureResource*, ConstCast(SpriteResource*, Value)));
			}

			bool ProgramConstantHolder::SetSprite(const String& Name, const SpriteResource* Value)
			{
				IMPLEMENT_SET_TEXTURE(GetHash(Name), ReinterpretCast(TextureResource*, ConstCast(SpriteResource*, Value)));
			}
		}
	}
}
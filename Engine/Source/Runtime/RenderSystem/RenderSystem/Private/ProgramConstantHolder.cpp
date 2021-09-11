// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\ProgramConstantHolder.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderSystem\ConstantBuffer.h>
#include <DataUtility\Hash.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace DataUtility;
	using namespace Debugging;
	using namespace RenderCommon::Private;

	namespace RenderSystem
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

			void ProgramConstantHolder::CreateBufferData(ProgramConstantHandle Handle, const String& Name, const String& UserDefinedType, uint16 Size)
			{
				ConstantBuffer* buffer = RenderSystemAllocators::ContainersAllocator_Allocate<ConstantBuffer>();
				Construct(buffer, Size);

				ConstantHash hash = GetHash(Name);

				CoreDebugAssert(Categories::RenderSystem, !m_Buffers.Contains(hash), "Attemp to add duplicate constant is forbidden");

				m_Buffers[hash] = BufferConstantData(Handle, Name, UserDefinedType, buffer);
			}

			void ProgramConstantHolder::CreateTextureData(ProgramConstantHandle Handle, const String& Name)
			{
				ConstantHash hash = GetHash(Name);

				CoreDebugAssert(Categories::RenderSystem, !m_Buffers.Contains(hash), "Attemp to add duplicate constant is forbidden");

				m_Textures[hash] = TextureConstantData(Handle, Name, nullptr);
			}

			void ProgramConstantHolder::CloneData(const ProgramConstantHolder& Other)
			{
				for (auto& bufferInfo : Other.m_Buffers)
				{
					auto data = bufferInfo.GetSecond();

					const auto otherBuffer = data.Value;

					data.Value = RenderSystemAllocators::ContainersAllocator_Allocate<ConstantBuffer>();
					Construct(data.Value, otherBuffer->GetSize());

					data.Value->Copy(otherBuffer);

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
					RenderSystemAllocators::ContainersAllocator_Deallocate(constant.GetSecond().Value);

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
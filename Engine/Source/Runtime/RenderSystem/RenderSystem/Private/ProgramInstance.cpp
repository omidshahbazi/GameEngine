// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\ProgramInstance.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\ConstantBuffer.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <DataUtility\Hash.h>

namespace Engine
{
	using namespace RenderCommon::Private;
	using namespace ProgramCompilerCommon;
	using namespace DataUtility;

	namespace RenderSystem
	{
		namespace Private
		{
#define IMPLEMENT_SET_TEXTURE(Hash, Pointer) \
		m_Textures[Hash] = Pointer; \
		return true;

			ProgramInstance::ProgramInstance(void) :
				m_Program(nullptr)
			{
			}

			ProgramInstance::ProgramInstance(ProgramResource* Program)
			{
				SetProgram(Program);
			}

			ProgramInstance::ProgramInstance(const ProgramInstance& Other)
			{
				*this = Other;
			}

			ProgramInstance::~ProgramInstance(void)
			{
				if (m_Program != nullptr)
					m_Program->OnUpdated -= EventListener_OnProgramUpdated;

				m_Program = nullptr;

				for (auto& constant : m_Buffers)
					RenderSystemAllocators::ContainersAllocator_Deallocate(constant.GetSecond());

				m_Buffers.Clear();
				m_Textures.Clear();
			}

			bool ProgramInstance::SetProgram(ProgramResource* Program)
			{
				if (m_Program == Program)
					return true;

				if (m_Program != nullptr)
					m_Program->OnUpdated -= EventListener_OnProgramUpdated;

				m_Program = Program;

				if (m_Program == nullptr)
					return true;

				m_Program->OnUpdated += EventListener_OnProgramUpdated;

				if (m_Program != nullptr)
					OnProgramUpdated(Program);
			}

			ConstantBuffer* ProgramInstance::GetConstantBuffer(ProgramConstantHash Hash, uint16 Size)
			{
				if (m_Buffers.Contains(Hash))
				{
					ConstantBuffer* buffer = m_Buffers[Hash];
					buffer->Resize(Size);
					return buffer;
				}

				ConstantBuffer* buffer = RenderSystemAllocators::ContainersAllocator_Allocate<ConstantBuffer>();
				Construct(buffer, Size);

				m_Buffers[Hash] = buffer;

				return buffer;
			}

			bool ProgramInstance::SetBuffer(ProgramConstantHash Hash, const byte* Data, uint16 Size)
			{
				GetConstantBuffer(Hash, Size)->Set(Data, Size);

				return true;
			}

			bool ProgramInstance::SetBuffer(ProgramConstantHash Hash, const ConstantBuffer* const Buffer)
			{
				GetConstantBuffer(Hash, Buffer->GetSize())->Copy(Buffer);

				return true;
			}

			bool ProgramInstance::SetTexture(ProgramConstantHash Hash, const TextureResource* Value)
			{
				IMPLEMENT_SET_TEXTURE(Hash, ConstCast(TextureResource*, Value));
			}

			bool ProgramInstance::SetTexture(const String& Name, const TextureResource* Value)
			{
				IMPLEMENT_SET_TEXTURE(GetHash(Name), ConstCast(TextureResource*, Value));
			}

			bool ProgramInstance::SetSprite(ProgramConstantHash Hash, const SpriteResource* Value)
			{
				IMPLEMENT_SET_TEXTURE(Hash, ReinterpretCast(TextureResource*, ConstCast(SpriteResource*, Value)));
			}

			bool ProgramInstance::SetSprite(const String& Name, const SpriteResource* Value)
			{
				IMPLEMENT_SET_TEXTURE(GetHash(Name), ReinterpretCast(TextureResource*, ConstCast(SpriteResource*, Value)));
			}

			ProgramInstance& ProgramInstance::operator=(const ProgramInstance& Other)
			{
				SetProgram(Other.m_Program);

				for (auto& bufferInfo : Other.m_Buffers)
				{
					auto hash = bufferInfo.GetFirst();
					auto otherValue = bufferInfo.GetSecond();

					ConstantBuffer* value = nullptr;
					if (m_Buffers.Contains(hash))
					{
						value = m_Buffers[hash];
						value->Resize(otherValue->GetSize());
					}
					else
					{
						m_Buffers[hash] = value = RenderSystemAllocators::ContainersAllocator_Allocate<ConstantBuffer>();
						Construct(value, otherValue->GetSize());
					}

					value->Copy(otherValue);
				}

				for (auto& textureInfo : Other.m_Textures)
					m_Textures[textureInfo.GetFirst()] = textureInfo.GetSecond();

				return *this;
			}

			void ProgramInstance::OnProgramUpdated(ProgramResource* Resource)
			{
				if (Resource->IsNull())
					return;

				auto& metaInfo = (*m_Program)->GetMetaInfo();

				m_Handles.Clear();
				for (auto& constant : metaInfo.Variables)
					m_Handles[GetHash(constant.Name)] = constant.Handle;
			}

			ProgramConstantHash ProgramInstance::GetHash(const String& Name)
			{
				return Hash::CRC32(Name.GetValue(), Name.GetLength());
			}
		}
	}
}
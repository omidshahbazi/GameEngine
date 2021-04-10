// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Program.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\ConstantBuffer.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace Rendering
	{
		Program::Program(ThreadedDevice* Device, Handle Handle, const MetaInfo& MetaInfo) :
			NativeType(Device, Handle),
			m_MetaInfo(MetaInfo)
		{
			GenerateConstantData();
		}

		Program::~Program(void)
		{
			for (auto& constant : m_BufferData)
				RenderingAllocators::ContainersAllocator_Deallocate(constant.GetSecond().Value);
		}

		void Program::SetName(const WString& Name)
		{
			NativeType::SetName(Name);

			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Program, GetName().GetValue());
		}

		ConstantBuffer* Program::GetConstantBuffer(ConstantHash Hash)
		{
			if (!m_BufferData.Contains(Hash))
				return nullptr;

			return m_BufferData[Hash].Value;
		}

		ConstantBuffer* Program::GetConstantBuffer(const String& Name)
		{
			return GetConstantBuffer(GetHash(Name));
		}

		bool Program::SetTexture(ConstantHash Hash, const TextureResource* Value)
		{
			if (!m_TextureData.Contains(Hash))
				return false;

			m_TextureData[Hash].Value = ConstCast(TextureResource*, Value);

			return true;
		}

		bool Program::SetTexture(const String& Name, const TextureResource* Value)
		{
			return SetTexture(GetHash(Name), Value);
		}

		bool Program::SetSprite(ConstantHash Hash, const SpriteResource* Value)
		{
			if (!m_TextureData.Contains(Hash))
				return false;

			m_TextureData[Hash].Value = ReinterpretCast(TextureResource*, ConstCast(SpriteResource*, Value));

			return true;
		}

		bool Program::SetSprite(const String& Name, const SpriteResource* Value)
		{
			return SetSprite(GetHash(Name), Value);
		}

		void Program::SetConstantsValue(IDevice* Device, const BufferInfoMap& Buffers, const TextureInfoMap& Texures)
		{
			for (auto& info : Buffers)
			{
				auto& constant = info.GetSecond();

				if (!m_BufferData.Contains(constant.Hash))
					continue;

				SetConstantBuffer(Device, constant.Value, m_BufferData[constant.Hash].Value);
			}

			for (auto& info : Texures)
			{
				auto& constant = info.GetSecond();

				if (!m_TextureData.Contains(constant.Hash))
					continue;

				m_TextureData[constant.Hash].Value = constant.Value;
			}
		}

		void Program::ApplyConstantsValue(IDevice* Device)
		{
			for (auto& info : m_BufferData)
			{
				auto& constant = info.GetSecond();

				if (constant.Value == nullptr)
					continue;

				Device->SetProgramConstantBuffer(constant.Handle, constant.Value->GetHandle());
			}

			for (auto& info : m_TextureData)
			{
				auto& constant = info.GetSecond();

				Texture::Handle texHandle = 0;
				Texture::Types type = Texture::Types::TwoD;
				if (constant.Value != nullptr && !constant.Value->IsNull())
				{
					Texture* tex = constant.Value->GetPointer();

					texHandle = tex->GetHandle();
					type = tex->GetType();
				}

				Device->SetProgramTexture(constant.Handle, type, texHandle);
			}
		}

		const StructMetaInfo* Program::GetStructInfoOf(Program::ConstantHash Hash) const
		{
			int32 index = m_MetaInfo.Variables.Find([Hash](auto& item) { return GetHash(item.Name) == Hash; });
			if (index == -1)
				return nullptr;

			const VariableMetaInfo& variableInfo = m_MetaInfo.Variables[index];

			index = m_MetaInfo.Structs.Find([&variableInfo](auto& item) { return item.Name == variableInfo.UserDefinedType; });
			if (index == -1)
				return nullptr;

			return &m_MetaInfo.Structs[index];
		}

		void Program::GenerateConstantData(void)
		{
			static const byte EMPTY_BUFFER[2048] = {};

			ConstantDataList list;
			GetDevice()->QueryProgramActiveConstants(GetHandle(), list).Wait();

			for (auto& constant : list)
			{
				if (constant.Type == ProgramDataTypes::Unknown)
				{
					const StructMetaInfo* structInfo = GetStructInfoOf(constant.Hash);
					if (structInfo == nullptr)
						return;

					GPUBuffer::Handle bufferHandle;
					if (!GetDevice()->CreateBuffer(bufferHandle).Wait())
						return;

					if (!GetDevice()->CopyDataToConstantBuffer(bufferHandle, EMPTY_BUFFER, structInfo->Size).Wait())
						return;

					ConstantBuffer* buffer = RenderingAllocators::RenderingSystemAllocator_Allocate<ConstantBuffer>();
					ConstructMacro(ConstantBuffer, buffer, this, structInfo->Size, bufferHandle);

					m_BufferData[constant.Hash] = BufferConstantData(constant.Handle, constant.Name, constant.UserDefinedType, buffer);

					continue;
				}

				m_TextureData[constant.Hash] = TextureConstantData(constant.Handle, constant.Name, constant.Type, nullptr);
			}
		}

		void Program::SetConstantBuffer(IDevice* Device, ConstantBuffer* SourceBuffer, ConstantBuffer* DestinationBuffer)
		{
			byte* data = nullptr;
			Device->LockBuffer(DestinationBuffer->GetHandle(), GPUBuffer::Types::Constant, GPUBuffer::Access::WriteOnly, &data);

			SourceBuffer->Lock(GPUBuffer::Access::ReadOnly);

			PlatformMemory::Copy(SourceBuffer->Get<byte>(), data, DestinationBuffer->GetSize());

			SourceBuffer->Unlock();

			Device->UnlockBuffer(DestinationBuffer->GetHandle(), GPUBuffer::Types::Constant);
		}
	}
}
// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Program.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\ConstantBuffer.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <ResourceSystem\Resource.h>




#include <Rendering\GPUAlignedType.h>

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
			for (auto& constant : m_ConstantsData)
			{
				if (constant.GetSecond().Type != ProgramDataTypes::Unknown)
					continue;

				RenderingAllocators::ContainersAllocator_Deallocate(constant.GetSecond().Value.Get<ConstantBuffer*>());
			}
		}

		void Program::SetName(const WString& Name)
		{
			NativeType::SetName(Name);

			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Program, GetName().GetValue());
		}

		ConstantBuffer* Program::GetConstantBuffer(ConstantHash Hash)
		{
			if (!m_ConstantsData.Contains(Hash))
				return nullptr;

			return m_ConstantsData[Hash].Value.Get<ConstantBuffer*>();
		}

		ConstantBuffer* Program::GetConstantBuffer(const String& Name)
		{
			return GetConstantBuffer(GetHash(Name));
		}

		bool Program::SetTexture(ConstantHash Hash, const TextureResource* Value)
		{
			if (!m_ConstantsData.Contains(Hash))
				return false;

			m_ConstantsData[Hash].Value = ReinterpretCast(void*, ConstCast(TextureResource*, Value));

			return true;
		}

		bool Program::SetTexture(const String& Name, const TextureResource* Value)
		{
			return SetTexture(GetHash(Name), Value);
		}

		bool Program::SetSprite(ConstantHash Hash, const SpriteResource* Value)
		{
			if (!m_ConstantsData.Contains(Hash))
				return false;

			m_ConstantsData[Hash].Value = ReinterpretCast(void*, ConstCast(SpriteResource*, Value));

			return true;
		}

		bool Program::SetSprite(const String& Name, const SpriteResource* Value)
		{
			return SetSprite(GetHash(Name), Value);
		}

		void Program::SetConstantsValue(const ConstantInfoMap& Constants)
		{
			for (auto& info : Constants)
			{
				auto& constant = info.GetSecond();

				if (!m_ConstantsData.Contains(constant.Hash))
					continue;

				m_ConstantsData[constant.Hash].Value = constant.Value;
			}
		}

		void Program::ApplyConstantsValue(IDevice* Device)
		{
			for (auto& item : m_ConstantsData)
			{
				auto& constant = item.GetSecond();

				SetConstantValueOnDevice(Device, constant.Handle, constant.Type, constant.UserDefinedType, constant.Value);
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

					constant.Value = buffer;




					struct DATA
					{
					public:
						GPUAlignedFloat32 time;
						GPUAlignedMatrix4F mvp;
						GPUAlignedMatrix4F view;
					};

					buffer->Lock(GPUBuffer::Access::ReadAndWrite);
					DATA* data = buffer->Get<DATA>();
					data->mvp = Matrix4F::Identity;
					data->time = 10.4F;
					buffer->Unlock();
				}

				m_ConstantsData[constant.Hash] = constant;
			}
		}

		bool Program::SetConstantValueOnDevice(IDevice* Device, Program::ConstantHandle Handle, ProgramDataTypes Type, const String& UserDefinedType, const AnyDataType& Value)
		{
			if (Type == ProgramDataTypes::Texture2D)
			{
				auto val = Value.Get<TextureResource*>();
				Texture::Types type = Texture::Types::TwoD;
				Texture::Handle texHandle = 0;
				if (val != nullptr && !val->IsNull())
				{
					Texture* tex = val->GetPointer();

					texHandle = tex->GetHandle();
					type = tex->GetType();
				}

				Device->SetProgramTexture(Handle, type, texHandle);

				return true;
			}

			if (UserDefinedType.GetLength() != 0)
			{
				auto val = Value.Get<ConstantBuffer*>();



				return true;
			}

			return false;
		}
	}
}
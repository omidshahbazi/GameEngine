// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Material.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\ConstantBuffer.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <ProgramCompilerCommon\OutputInfo.h>
#include <Platform\PlatformMemory.h>
#include <DataUtility\Hash.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace RenderCommon::Private;
	using namespace ProgramCompilerCommon;
	using namespace Platform;
	using namespace DataUtility;
	using namespace Debugging;

	namespace RenderSystem
	{
		using namespace Private;

#define IMPLEMENT_SET_TEXTURE(Hash, Pointer) \
		if (!m_Textures.Contains(Hash)) \
			return false; \
		m_Textures[Hash].Value = Pointer; \
		return true;

		const StructMetaInfo* FindStructInfoOf(const MetaInfo& MetaInfo, const String& VariableName)
		{
			ProgramConstantHash hash = Material::GetHash(VariableName);

			int32 index = MetaInfo.Variables.FindIf([hash](auto& item) { return Material::GetHash(item.Name) == hash; });
			if (index == -1)
				return nullptr;

			const VariableMetaInfo& variableInfo = MetaInfo.Variables[index];

			index = MetaInfo.Structs.FindIf([&variableInfo](auto& item) { return item.Name == variableInfo.UserDefinedType; });
			if (index == -1)
				return nullptr;

			return &MetaInfo.Structs[index];
		}

		Material::Material(void) :
			m_Program(nullptr),
			m_Queue(RenderQueues::Default)
		{
		}

		Material::Material(ProgramResource* Program) :
			m_Queue(RenderQueues::Default)
		{
			SetProgram(Program);
		}

		Material::Material(const Material& Other)
		{
			*this = Other;
		}

		Material::Material(Material&& Other)
		{
			*this = std::move(Other);
		}

		Material::~Material(void)
		{
			m_Program = nullptr;

			for (auto& constant : m_Buffers)
				RenderSystemAllocators::ContainersAllocator_Deallocate(constant.GetSecond().Value);

			m_Buffers.Clear();
			m_Textures.Clear();
		}

		bool Material::SetProgram(ProgramResource* Program)
		{
			if (m_Program == Program)
				return true;

			m_Program = Program;

			if (m_Program == nullptr)
				return true;

			CoreDebugAssert(Categories::RenderSystem, !m_Program->IsNull(), "Program cannot be null to initialize a Pass");

			//just sync not from scratch

			auto& metaInfo = (*m_Program)->GetMetaInfo();

			for (auto& constant : metaInfo.Variables)
			{
				if (constant.DataType == ProgramDataTypes::Unknown)
				{
					const StructMetaInfo* structInfo = FindStructInfoOf(metaInfo, constant.Name);
					if (structInfo == nullptr)
						return false;

					CreateBufferData(constant.Handle, constant.Name, constant.UserDefinedType, structInfo->Size);

					continue;
				}

				CreateTextureData(constant.Handle, constant.Name);
			}

			return true;
		}

		ConstantBuffer* Material::GetConstantBuffer(ProgramConstantHash Hash)
		{
			ConstantBuffer* buffer = nullptr;

			if (m_Buffers.Contains(Hash))
				return m_Buffers[Hash].Value;

			return nullptr;
		}

		ConstantBuffer* Material::GetConstantBuffer(const String& Name)
		{
			return GetConstantBuffer(GetHash(Name));
		}

		bool Material::SetBuffer(ProgramConstantHash Hash, const byte* Data, uint16 Size)
		{
			ConstantBuffer* buffer = GetConstantBuffer(Hash);
			if (buffer == nullptr)
				return false;

			buffer->Set(Data, Size);

			return true;
		}

		bool Material::SetTexture(ProgramConstantHash Hash, const TextureResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(Hash, ConstCast(TextureResource*, Value));
		}

		bool Material::SetTexture(const String& Name, const TextureResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(GetHash(Name), ConstCast(TextureResource*, Value));
		}

		bool Material::SetSprite(ProgramConstantHash Hash, const SpriteResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(Hash, ReinterpretCast(TextureResource*, ConstCast(SpriteResource*, Value)));
		}

		bool Material::SetSprite(const String& Name, const SpriteResource* Value)
		{
			IMPLEMENT_SET_TEXTURE(GetHash(Name), ReinterpretCast(TextureResource*, ConstCast(SpriteResource*, Value)));
		}

		void Material::SetRenderState(const RenderState& State)
		{
			PlatformMemory::Copy(&State, &m_RenderState, 1);
		}

		Material& Material::operator=(const Material& Other)
		{
			m_Program = Other.m_Program;
			m_Queue = Other.m_Queue;
			m_RenderState = Other.m_RenderState;

			SyncData(Other);

			return *this;
		}

		void Material::CreateBufferData(ProgramConstantHandle Handle, const String& Name, const String& UserDefinedType, uint16 Size)
		{
			ConstantBuffer* buffer = RenderSystemAllocators::ContainersAllocator_Allocate<ConstantBuffer>();
			Construct(buffer, Size);

			ProgramConstantHash hash = GetHash(Name);

			CoreDebugAssert(Categories::RenderSystem, !m_Buffers.Contains(hash), "Attempt to add duplicate constant is forbidden");

			m_Buffers[hash] = BufferMetaConstantData(Handle, Name, hash, UserDefinedType, buffer);
		}

		void Material::CreateTextureData(ProgramConstantHandle Handle, const String& Name)
		{
			ProgramConstantHash hash = GetHash(Name);

			CoreDebugAssert(Categories::RenderSystem, !m_Buffers.Contains(hash), "Attempt to add duplicate constant is forbidden");

			m_Textures[hash] = TextureMetaConstantData(Handle, Name, hash, nullptr);
		}

		void Material::SyncData(const Material& Other)
		{
			for (auto& bufferInfo : Other.m_Buffers)
			{
				auto& otherData = bufferInfo.GetSecond();
				bool contains = m_Buffers.Contains(otherData.Hash);
				auto& selfData = m_Buffers[otherData.Hash];

				auto selfValue = selfData.Value;

				auto value = otherData.Value;
				if (contains)
					value = selfValue;

				selfData = otherData;

				selfData.Value = RenderSystemAllocators::ContainersAllocator_Allocate<ConstantBuffer>();
				Construct(selfData.Value, otherData.Value->GetSize());
				selfData.Value->Copy(value);

				if (contains)
					RenderSystemAllocators::ContainersAllocator_Deallocate(selfValue);
			}

			m_Buffers.RemoveIf([&Other](auto Item)
				{
					bool goingToRemove = !Other.m_Buffers.Contains(Item.GetFirst());

					if (goingToRemove)
						RenderSystemAllocators::ContainersAllocator_Deallocate(Item.GetSecond().Value);

					return goingToRemove;
				});

			for (auto& textureInfo : Other.m_Textures)
			{
				auto& otherData = textureInfo.GetSecond();
				bool contains = m_Textures.Contains(otherData.Hash);
				auto& selfData = m_Textures[otherData.Hash];

				auto value = otherData.Value;
				if (contains)
					value = selfData.Value;

				selfData = otherData;
				selfData.Value = value;
			}
			m_Textures.RemoveIf([&Other](auto Item) { return !Other.m_Textures.Contains(Item.GetFirst()); });
		}

		ProgramConstantHash Material::GetHash(const String& Name)
		{
			return Hash::CRC32(Name.GetValue(), Name.GetLength());
		}
	}
}
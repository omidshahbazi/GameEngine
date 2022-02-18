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

		const StructMetaInfo* FindStructInfoOf(const MetaInfo& MetaInfo, ProgramConstantHash VariableHash)
		{
			int32 index = MetaInfo.Variables.FindIf([VariableHash](auto& item) { return Material::GetHash(item.Name) == VariableHash; });
			if (index == -1)
				return nullptr;

			const VariableMetaInfo& variableInfo = MetaInfo.Variables[index];

			index = MetaInfo.Structs.FindIf([&variableInfo](auto& item) { return item.Name == variableInfo.UserDefinedType; });
			if (index == -1)
				return nullptr;

			return &MetaInfo.Structs[index];
		}

		void CreateBufferData(Material::BufferMetaDataMap& Map, ProgramConstantHandle Handle, const String& Name, const String& UserDefinedType, uint16 Size)
		{
			ConstantBuffer* buffer = RenderSystemAllocators::ContainersAllocator_Allocate<ConstantBuffer>();
			Construct(buffer, Size);

			ProgramConstantHash hash = Material::GetHash(Name);

			CoreDebugAssert(Categories::RenderSystem, !Map.Contains(hash), "Attempt to add duplicate constant is forbidden");

			Map[hash] = Material::BufferMetaConstantData(Handle, Name, hash, UserDefinedType, buffer);
		}

		void CreateTextureData(Material::TextureMetaDataMap& Map, ProgramConstantHandle Handle, const String& Name)
		{
			ProgramConstantHash hash = Material::GetHash(Name);

			CoreDebugAssert(Categories::RenderSystem, !Map.Contains(hash), "Attempt to add duplicate constant is forbidden");

			Map[hash] = Material::TextureMetaConstantData(Handle, Name, hash, nullptr);
		}

		template<typename T>
		void Remove(Map<ProgramConstantHash, T> Map, std::function<bool(ProgramConstantHash)> Condition, bool DeallocateValue)
		{
			Map.RemoveIf([&](auto Item)
				{
					bool goingToRemove = Condition(Item.GetFirst());

					if (goingToRemove && DeallocateValue)
						RenderSystemAllocators::ContainersAllocator_Deallocate(Item.GetSecond().Value);

					return goingToRemove;
				});
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

			auto& metaInfo = (*m_Program)->GetMetaInfo();

			for (auto& constant : metaInfo.Variables)
			{
				ProgramConstantHash hash = GetHash(constant.Name);

				if (constant.DataType == ProgramDataTypes::Unknown)
				{
					const StructMetaInfo* structInfo = FindStructInfoOf(metaInfo, hash);
					if (structInfo == nullptr)
						return false;

					if (m_Buffers.Contains(hash))
					{
						const BufferMetaConstantData& data = m_Buffers[hash];
						if (data.Value->GetSize() == structInfo->Size)
							continue;

						RenderSystemAllocators::ContainersAllocator_Deallocate(data.Value);
					}

					CreateBufferData(m_Buffers, constant.Handle, constant.Name, constant.UserDefinedType, structInfo->Size);

					continue;
				}
				else
				{
					if (m_Textures.Contains(hash))
						continue;

					CreateTextureData(m_Textures, constant.Handle, constant.Name);
				}
			}

			Remove(m_Buffers, [&metaInfo](ProgramConstantHash Hash) { return !metaInfo.Variables.ContainsIf([Hash](auto& item) { return Material::GetHash(item.Name) == Hash; }); }, true);
			Remove(m_Textures, [&metaInfo](ProgramConstantHash Hash) { return !metaInfo.Variables.ContainsIf([Hash](auto& item) { return Material::GetHash(item.Name) == Hash; }); }, false);
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

			Remove(m_Buffers, [&Other](ProgramConstantHash Hash) { return !Other.m_Buffers.Contains(Hash); }, true);

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

			Remove(m_Textures, [&Other](ProgramConstantHash Hash) { return !Other.m_Textures.Contains(Hash); }, false);

			return *this;
		}

		ProgramConstantHash Material::GetHash(const String& Name)
		{
			return Hash::CRC32(Name.GetValue(), Name.GetLength());
		}
	}
}
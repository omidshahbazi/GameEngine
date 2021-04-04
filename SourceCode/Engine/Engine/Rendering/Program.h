// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H

#include <Rendering\NativeType.h>
#include <Rendering\CompiledProgramInfo.h>
#include <Rendering\Private\ProgramConstantHolder.h>

namespace Engine
{
	using namespace ResourceSystem;
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class DrawCommand;
			}
		}

		class DeviceInterface;
		class ConstantBuffer;
		class Pass;

		using namespace Private::Commands;

		class RENDERING_API Program : public NativeType, public ProgramConstantHolder
		{
			friend class DeviceInterface;
			friend class ConstantBuffer;
			friend class ProgramConstantSupplier;
			friend class DrawCommand;
			friend class Pass;

		private:
			Program(ThreadedDevice* Device, Handle Handle, const MetaInfo& Meta);

		public:
			virtual ~Program(void);

			void SetName(const WString& Name) override;

			ConstantBuffer* GetConstantBuffer(ConstantHash Hash) override;
			ConstantBuffer* GetConstantBuffer(const String& Name) override;

			bool SetTexture(ConstantHash Hash, const TextureResource* Value) override;
			bool SetTexture(const String& Name, const TextureResource* Value) override;

			bool SetSprite(ConstantHash Hash, const SpriteResource* Value) override;
			bool SetSprite(const String& Name, const SpriteResource* Value) override;

		private:
			void SetConstantsValue(const BufferInfoMap& Buffers, const TextureInfoMap& Texures);

			void ApplyConstantsValue(IDevice* Device);

			INLINE const BufferDataMap& GetBuffers(void) const
			{
				return m_BufferData;
			}

			INLINE const TextureDataMap& GetTextures(void) const
			{
				return m_TextureData;
			}

			INLINE const StructMetaInfo* GetStructInfoOf(ConstantHash Handle) const;

			void GenerateConstantData(void);

		private:
			BufferDataMap m_BufferData;
			TextureDataMap m_TextureData;
			MetaInfo m_MetaInfo;
		};
	}
}

#endif
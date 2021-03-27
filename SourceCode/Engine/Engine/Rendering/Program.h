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
			void SetConstantsValue(const ConstantInfoMap& Constants);

			void ApplyConstantsValue(IDevice* Device);

			INLINE ConstantDataMap& GetConstants(void)
			{
				return m_ConstantsData;
			}

			INLINE const ConstantDataMap& GetConstants(void) const
			{
				return m_ConstantsData;
			}

			INLINE const StructMetaInfo* GetStructInfoOf(ConstantHash Handle) const;

			void GenerateConstantData(void);

			static bool SetConstantValueOnDevice(IDevice* Device, ConstantHandle Handle, ProgramDataTypes Type, const String& UserDefinedType, const AnyDataType& Value);

		private:
			ConstantDataMap m_ConstantsData;
			MetaInfo m_MetaInfo;
		};
	}
}

#endif
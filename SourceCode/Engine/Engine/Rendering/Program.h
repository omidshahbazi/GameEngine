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
		class Pass;

		using namespace Private::Commands;

		class RENDERING_API Program : public NativeType, public ProgramConstantHolder
		{
			friend class DeviceInterface;
			friend class ProgramConstantSupplier;
			friend class DrawCommand;
			friend class Pass;

		private:
			Program(ThreadedDevice* Device, Handle Handle, const MetaInfo& Meta);

		public:
			virtual void SetName(const WString& Name) override;

			bool SetFloat32(ConstantHash Hash, float32 Value) override;
			bool SetColor(ConstantHash Hash, const ColorUI8& Value) override;
			bool SetVector2(ConstantHash Hash, const Vector2F& Value) override;
			bool SetVector3(ConstantHash Hash, const Vector3F& Value) override;
			bool SetVector4(ConstantHash Hash, const Vector4F& Value) override;
			bool SetMatrix4(ConstantHash Hash, const Matrix4F& Value) override;
			bool SetTexture(ConstantHash Hash, const TextureResource* Value) override;
			bool SetSprite(ConstantHash Hash, const SpriteResource* Value) override;

			bool SetFloat32(const String& Name, float32 Value) override;
			bool SetColor(const String& Name, const ColorUI8& Value) override;
			bool SetVector2(const String& Name, const Vector2F& Value) override;
			bool SetVector3(const String& Name, const Vector3F& Value) override;
			bool SetVector4(const String& Name, const Vector4F& Value) override;
			bool SetMatrix4(const String& Name, const Matrix4F& Value) override;
			bool SetTexture(const String& Name, const TextureResource* Value) override;
			bool SetSprite(const String& Name, const SpriteResource* Value) override;

			ConstantHash GetConstantHash(const String& Name);

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

			bool SetConstantValue(ConstantHash Handle, const AnyDataType& Value);

			void QueryActiveConstants(void);

			static bool SetConstantValueOnDevice(IDevice* Device, ConstantHandle Handle, ProgramDataTypes Type, const AnyDataType& Value);

		private:
			ConstantDataMap m_ConstantsData;
			MetaInfo m_MetaInfo;
		};
	}
}

#endif
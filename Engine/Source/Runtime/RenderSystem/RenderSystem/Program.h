// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H

#include <RenderSystem\NativeType.h>
#include <RenderSystem\Private\ProgramConstantHolder.h>
#include <ProgramCompilerCommon\OutputInfo.h>

namespace Engine
{
	using namespace ProgramCompilerCommon;

	namespace RenderSystem
	{
		class DeviceInterface;

		using namespace Private;

		class RENDERSYSTEM_API Program : public NativeType, public ProgramConstantHolder
		{
			friend class DeviceInterface;

		private:
			Program(ThreadedDevice* Device, ResourceHandle Handle, const MetaInfo& Meta);

			void SetNameInternal(const WString& Name) override;

			const StructMetaInfo* GetStructInfoOf(const String& VariableName) const;

			void GenerateConstantData(void);

		private:
			MetaInfo m_MetaInfo;
		};
	}
}

#endif
// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H

#include <Rendering\NativeType.h>
#include <Rendering\CompiledProgramInfo.h>
#include <Rendering\Private\ProgramConstantHolder.h>

namespace Engine
{
	namespace Rendering
	{
		class DeviceInterface;

		using namespace Private::Commands;

		class RENDERING_API Program : public NativeType, public ProgramConstantHolder
		{
			friend class DeviceInterface;

		private:
			Program(ThreadedDevice* Device, Handle Handle, const MetaInfo& Meta);

		public:
			void SetName(const WString& Name) override;

		private:
			const StructMetaInfo* GetStructInfoOf(const String& VariableName) const;

			void GenerateConstantData(void);

		private:
			MetaInfo m_MetaInfo;
		};
	}
}

#endif
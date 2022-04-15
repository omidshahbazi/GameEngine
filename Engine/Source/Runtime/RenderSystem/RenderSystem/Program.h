// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_H
#define PROGRAM_H

#include <RenderSystem\NativeType.h>
#include <ProgramCompilerCommon\OutputInfo.h>

namespace Engine
{
	using namespace ProgramCompilerCommon;

	namespace RenderSystem
	{
		class DeviceInterface;

		namespace Private
		{
			class ProgramInstance;
		}

		using namespace Private;

		class RENDERSYSTEM_API Program : public NativeType
		{
			friend class DeviceInterface;
			friend class ProgramInstance;

		private:
			Program(ThreadedDevice* Device, ResourceHandle Handle, const MetaInfo& Meta);

			void SetNameInternal(const WString& Name) override;

			const MetaInfo& GetMetaInfo(void) const
			{
				return m_MetaInfo;
			}

		private:
			MetaInfo m_MetaInfo;
		};
	}
}

#endif
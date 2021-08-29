// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INTRINSIC_CONSTANTS_H
#define INTRINSIC_CONSTANTS_H

#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <Rendering\RenderingCommon.h>
#include <Rendering\ProgramDataTypes.h>
#include <Rendering\Private\ProgramCompiler\Common.h>
#include <Rendering\Private\ProgramCompiler\Syntax\FunctionType.h>
#include <memory>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				using namespace Syntax;

				class IntrinsicConstants
				{
				private:
					struct ConstantInfo
					{
					public:
						typedef std::function<void(FunctionType::Types, Stages, String&)> BuildStatement;

					public:
						ProgramDataTypes Type;

						std::shared_ptr<BuildStatement> Build;
					};

					typedef Map<String, ConstantInfo> ConstantMap;

				protected:
					IntrinsicConstants(DeviceTypes Device);

					bool BuildIntrinsicConstantStatement(const String& Name, FunctionType::Types Type, Stages Stage, String& Shader);

					virtual void BuildType(ProgramDataTypes Type, String& Shader) = 0;
					virtual void InjectParameterIntoTopFunction(ProgramDataTypes Type, const String& Name, const String& Register) = 0;

				private:
					ConstantMap m_Constants;
				};
			}
		}
	}
}

#endif
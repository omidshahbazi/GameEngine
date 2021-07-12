// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INTRINSIC_FUNCTIONS_H
#define INTRINSIC_FUNCTIONS_H

#include <Containers\Strings.h>
#include <Containers\Vector.h>
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
				namespace Syntax
				{
					class Statement;
					class FunctionCallStatement;
				}

				using namespace Syntax;

				class IntrinsicFunctions
				{
				private:
					static const uint8 MAX_PARAMETER_COUNT = 8;

					struct FunctionInfo
					{

					public:
						struct NativeFunctionDescription
						{
						public:
							typedef std::function<void(FunctionCallStatement*, FunctionType::Types, Stages, String&)> CustomBuildStatement;

						public:
							String Name;
							std::shared_ptr<CustomBuildStatement> BuildCustom;
						};

					public:
						String Name;
						ProgramDataTypes ReturnType;
						ProgramDataTypes ParameterTypes[MAX_PARAMETER_COUNT];
						uint8 ParameterTypeCount;
						uint32 Hash;

						NativeFunctionDescription Descriptions[DEVICE_TYPE_COUNT];
					};

					typedef Vector<FunctionInfo> FunctionOverrideInfoList;
					typedef Map<String, FunctionOverrideInfoList> FunctionMap;

				protected:
					IntrinsicFunctions(void);

				protected:
					bool BuildIntrinsicFunctionCallStatement(DeviceTypes Device, FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader);

					virtual ProgramDataTypes EvaluateProgramDataType(Statement* Statement) = 0;
					virtual void BuildStatement(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					//Evaluate ? !

				private:
					FunctionMap m_Functions;
				};
			}
		}
	}
}

#endif
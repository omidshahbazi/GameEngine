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
#include <Rendering\Private\ProgramCompiler\Syntax\FunctionCallStatement.h>
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
				}

				using namespace Syntax;

				class IntrinsicFunctions
				{
				private:
					struct FunctionInfo
					{
					public:
						typedef std::function<void(const Vector<Statement*>&, FunctionType::Types, Stages, String&)> CustomBuildStatement;

					public:
						ProgramDataTypes ReturnType;
						uint32 Hash;

						String NativeFunctionName;
						std::shared_ptr<CustomBuildStatement> BuildCustom;
					};

					typedef Vector<FunctionInfo> FunctionOverrideInfoList;
					typedef Map<String, FunctionOverrideInfoList> FunctionMap;

				protected:
					IntrinsicFunctions(DeviceTypes Device);

					bool BuildIntrinsicFunctionCallStatement(const String& Name, const Vector<Statement*>& Arguments, FunctionType::Types Type, Stages Stage, String& Shader);

					bool BuildIntrinsicFunctionCallStatement(FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						return BuildIntrinsicFunctionCallStatement(Statement->GetFunctionName(), Statement->GetArguments()->GetItems(), Type, Stage, Shader);
					}

					ProgramDataTypes EvaluateIntrinsicFunctionReturnValue(FunctionCallStatement* Statement) const;

					uint32 CalculateFunctionSignatureHash(const String& Name, const Vector<Statement*>& Arguments) const;

				private:
					const IntrinsicFunctions::FunctionInfo* FindOverride(const String& Name, const Vector<Statement*>& Arguments) const;

					virtual ProgramDataTypes EvaluateProgramDataType(Statement* Statement) const = 0;
					virtual void BuildStatement(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;
					virtual void BuildArguments(const Vector<Statement*>& Statements, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

				protected:
					static uint32 CalculateFunctionSignatureHash(const String& Name, const ProgramDataTypes* ParameterTypes, uint8 ParameterTypeCount);

				private:
					FunctionMap m_Functions;
				};
			}
		}
	}
}

#endif
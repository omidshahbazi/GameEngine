// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_INTRINSIC_PROVIDER_H
#define I_INTRINSIC_PROVIDER_H

#include <Containers\Strings.h>
#include <Containers\Vector.h>
#include <Containers\Map.h>
#include <ProgramParser\AbstractSyntaxTree\FunctionType.h>
#include <ProgramCompilerCommon\Common.h>
#include <Debugging\CoreDebug.h>
#include <functional>
#include <memory>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class Statement;
		}
	}

	using namespace Containers;
	using namespace ProgramCompilerCommon;
	using namespace ProgramParser;
	using namespace ProgramParser::AbstractSyntaxTree;
	using namespace Debugging;


	namespace APIIntrinsic
	{
		class IIntrinsicProvider
		{
		public:
			class IInitializeHelper
			{
			public:
				virtual uint32 GetFunctionSignatureHash(const String& Name, const ProgramDataTypes* ParameterTypes, uint8 ParameterTypeCount) = 0;
			};

			class ICompiler
			{
			public:
				virtual void InjectParameterIntoTopFunction(ProgramDataTypes Type, const String& Name, const String& Register) = 0;
				virtual void BuildStatement(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;
				virtual void BuildArguments(const Vector<Statement*>& Statements, FunctionType::Types Type, Stages Stage, String& Shader) = 0;
				virtual void BuildType(ProgramDataTypes Type, String& Shader) = 0;
				virtual ProgramDataTypes EvaluateProgramDataType(Statement* Statement) const = 0;
			};

			struct ConstantInfo
			{
			public:
				typedef std::function<void(ICompiler*, FunctionType::Types, Stages, String&)> BuildStatement;

			public:
				ProgramDataTypes Type;

				std::shared_ptr<BuildStatement> Build;
			};

			struct FunctionInfo
			{
			public:
				typedef std::function<void(ICompiler*, const Vector<Statement*>&, FunctionType::Types, Stages, String&)> CustomBuildStatement;

			public:
				ProgramDataTypes ReturnType;
				uint32 Hash;

				String NativeFunctionName;
				std::shared_ptr<CustomBuildStatement> BuildCustom;
			};

			typedef Map<String, ConstantInfo> ConstantMap;
			typedef Vector<FunctionInfo> FunctionOverrideInfoList;
			typedef Map<String, FunctionOverrideInfoList> FunctionMap;

		public:
			virtual void Initialize(IInitializeHelper* Helper) = 0;

			virtual const ConstantMap& GetConstants(void) const = 0;
			virtual const FunctionMap& GetFunctions(void) const = 0;
		};

		const uint8 MAX_PARAMETER_COUNT = 16;

#define ADD_CONSTANT(Name, DataType, BuildCallback) \
		{ \
			const String name = Name; \
			CoreDebugAssert(Categories::ProgramCompiler, !m_Constants.Contains(name), "Constant already added"); \
			ConstantInfo& info = m_Constants[name]; \
			info.Type = DataType; \
			info.Build = std::make_shared<ConstantInfo::BuildStatement>(BuildCallback); \
		}

#define BEGIN_FUNCTION(Name, OverrideCount) \
		{ \
			const String name = Name; \
			CoreDebugAssert(Categories::ProgramCompiler, !m_Functions.Contains(name), "Function already added"); \
			FunctionOverrideInfoList& list = m_Functions[name]; \
			list.Extend(OverrideCount); \
			uint8 overrideIndex = 0;

#define BEGIN_OVERRIDE(ReturnDataType) \
			{ \
				FunctionInfo& function = list[overrideIndex++]; \
				function.ReturnType = ReturnDataType; \
				ProgramDataTypes parameterTypes[MAX_PARAMETER_COUNT]; \
				uint8 parameterTypeCount = 0;

#define ADD_PARAMETER(DataType) \
				parameterTypes[parameterTypeCount++] = DataType;

#define SET_NATIVE_DESCRIPTION(NativeName) \
				function.NativeFunctionName = NativeName; \
				function.BuildCustom = nullptr;

#define SET_CUSTOM_NATIVE_DESCRIPTION(CustomBuildCallback) \
				function.NativeFunctionName = ""; \
				function.BuildCustom = std::make_shared<FunctionInfo::CustomBuildStatement>(CustomBuildCallback);

#define END_OVERRIDE() \
				function.Hash = Helper->GetFunctionSignatureHash(name, parameterTypes, parameterTypeCount); \
			}

#define END_FUNCTION() \
		}
	}
}

#endif
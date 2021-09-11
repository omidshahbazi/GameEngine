// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INTRINSIC_FUNCTIONS_H
#define INTRINSIC_FUNCTIONS_H

#include <APIIntrinsic\IIntrinsicProvider.h>
#include <ProgramParser\AbstractSyntaxTree\FunctionCallStatement.h>

namespace Engine
{
	using namespace Containers;
	using namespace APIIntrinsic;
	using namespace ProgramCompilerCommon;
	using namespace ProgramParser;
	using namespace ProgramParser::AbstractSyntaxTree;

	namespace APIIntrinsic
	{
		class APIINTRINSIC_API IntrinsicsBuilder : public IIntrinsicProvider::IInitializeHelper, public IIntrinsicProvider::ICompiler
		{
		protected:
			IntrinsicsBuilder(void);
			virtual ~IntrinsicsBuilder(void);

			void Initialize(DeviceTypes Device);

			bool BuildConstantStatement(const String& Name, FunctionType::Types Type, Stages Stage, String& Shader);

			bool BuildFunctionCallStatement(const String& Name, const Vector<Statement*>& Arguments, FunctionType::Types Type, Stages Stage, String& Shader);

			bool BuildFunctionCallStatement(FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				return BuildFunctionCallStatement(Statement->GetFunctionName(), Statement->GetArguments()->GetItems(), Type, Stage, Shader);
			}

			ProgramDataTypes EvaluateFunctionReturnValue(FunctionCallStatement* Statement) const;

			uint32 CalculateFunctionSignatureHash(const String& Name, const Vector<Statement*>& Arguments) const;

		private:
			const IIntrinsicProvider::FunctionInfo* FindOverride(const String& Name, const Vector<Statement*>& Arguments) const;

			uint32 GetFunctionSignatureHash(const String& Name, const ProgramDataTypes* ParameterTypes, uint8 ParameterTypeCount) override
			{
				return CalculateFunctionSignatureHash(Name, ParameterTypes, ParameterTypeCount);
			}

		protected:
			static uint32 CalculateFunctionSignatureHash(const String& Name, const ProgramDataTypes* ParameterTypes, uint8 ParameterTypeCount);

		private:
			IIntrinsicProvider* m_Provider;
		};
	}
}

#endif
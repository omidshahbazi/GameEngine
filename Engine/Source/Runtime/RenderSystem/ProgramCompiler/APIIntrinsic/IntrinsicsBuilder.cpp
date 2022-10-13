// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <APIIntrinsic\IntrinsicsBuilder.h>
#include <DynamicModuleSystem\ModuleManager.h>
#include <Debugging\CoreDebug.h>
#include <DataUtility\Hash.h>

namespace Engine
{
	using namespace DataUtility;
	using namespace DynamicModuleSystem;

	namespace APIIntrinsic
	{
		IntrinsicsBuilder::IntrinsicsBuilder(void) :
			m_Provider(nullptr)
		{
		}

		IntrinsicsBuilder::~IntrinsicsBuilder(void)
		{
			ModuleManager::GetInstance()->Unload(m_Provider).Wait();
		}

		void IntrinsicsBuilder::Initialize(DeviceTypes Device)
		{
			if (m_Provider != nullptr)
				return;

			String moduleName = "";

			switch (Device)
			{
			case DeviceTypes::OpenGL:
			case DeviceTypes::Vulkan:
				moduleName = "GLSLIntrinsic";
				break;

			case DeviceTypes::DirectX12:
				moduleName = "HLSLIntrinsic";
				break;

			default:
				CoreDebugAssert(Categories::ProgramCompiler, false, "Device type is not supported");
			}

			m_Provider = ModuleManager::GetInstance()->Load<IIntrinsicProvider>(moduleName);
			CoreDebugAssert(Categories::ProgramCompiler, m_Provider != nullptr, "Couldn't load %S module", moduleName);

			m_Provider->Initialize(this);
		}

		bool IntrinsicsBuilder::BuildFunctionCallStatement(const String& Name, const Vector<Statement*>& Arguments, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			auto function = FindOverride(Name, Arguments);
			if (function == nullptr)
				return false;

			if (function->BuildCustom == nullptr)
			{
				Shader += function->NativeFunctionName;
				Shader += '(';

				BuildArguments(Arguments, Type, Stage, Shader);

				Shader += ')';
			}
			else
				(*function->BuildCustom)(this, Arguments, Type, Stage, Shader);

			return true;
		}

		ProgramDataTypes IntrinsicsBuilder::EvaluateFunctionReturnValue(FunctionCallStatement* Statement) const
		{
			auto function = FindOverride(Statement->GetFunctionName(), Statement->GetArguments()->GetItems());
			if (function == nullptr)
				return ProgramDataTypes::Unknown;

			return function->ReturnType;
		}

		uint32 IntrinsicsBuilder::CalculateFunctionSignatureHash(const String& Name, const Vector<Statement*>& Arguments) const
		{
			ProgramDataTypes parameterTypes[MAX_PARAMETER_COUNT];
			uint8 parameterTypeCount = 0;

			for (auto& statement : Arguments)
				parameterTypes[parameterTypeCount++] = EvaluateProgramDataType(statement);

			return CalculateFunctionSignatureHash(Name, parameterTypes, parameterTypeCount);
		}

		const IIntrinsicProvider::FunctionInfo* IntrinsicsBuilder::FindOverride(const String& Name, const Vector<Statement*>& Arguments) const
		{
			const auto& functions = m_Provider->GetFunctions();

			if (!functions.Contains(Name))
				return nullptr;

			uint32 hash = CalculateFunctionSignatureHash(Name, Arguments);

			auto& overrides = functions[Name];
			int32 index = overrides.FindIf([hash](auto item) { return item.Hash == hash; });
			if (index != -1)
				return &overrides[index];

			index = overrides.FindIf([hash](auto item) { return item.IsTemplate; });
			if (index != -1)
				return &overrides[index];

			return nullptr;
		}

		uint32 IntrinsicsBuilder::CalculateFunctionSignatureHash(const String& Name, const ProgramDataTypes* ParameterTypes, uint8 ParameterTypeCount)
		{
			return Hash::CRC32(Name.GetValue(), Name.GetLength()) + Hash::CRC32(ParameterTypes, ParameterTypeCount);
		}
	}
}
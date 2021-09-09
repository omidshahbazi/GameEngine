// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ProgramCompiler\ProgramToAPICompiler.h>
#include <ProgramCompilerCommon\ProgramCompilerException.h>
#include <ProgramParser\PreprocessorParser.h>
#include <ProgramParser\Parser.h>
#include <ProgramParser\AbstractSyntaxTree\FunctionType.h>
#include <ASTCompiler\ASTCompilerBase.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>
#include <DynamicModuleSystem\ModuleManager.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace ProgramCompilerCommon;
	using namespace ProgramParser;
	using namespace RenderCommon::Private;
	using namespace MemoryManagement::Allocator;
	using namespace ProgramParser::AbstractSyntaxTree;
	using namespace ASTCompiler;
	using namespace DynamicModuleSystem;
	using namespace Debugging;

	namespace ProgramCompiler
	{
		SINGLETON_DEFINITION(ProgramToAPICompiler);

		void ProgramToAPICompiler::Compile(const ProgramInfo* Info, DeviceTypes Type, OutputInfo& Output)
		{
			Compile(Info, &Type, 1, &Output);
		}

		void ProgramToAPICompiler::Compile(const ProgramInfo* Info, const DeviceTypes* Types, uint8 TypeCount, OutputInfo* Outputs)
		{
			if (Info->Source.GetLength() == 0)
				THROW_PROGRAM_COMPILER_EXCEPTION("Program source cannot be empty", String::Empty);

			PreprocessorParser preprocessorParser(Info->Source);
			PreprocessorParser::Parameters preprocessParameters;
			preprocessParameters.IncludeFunction = [&](const String& Name, String& Source)
			{
				for (auto handler : OnFetchShaderSourceEvent.GetHandlers())
				{
					(*handler)(Name, Source);

					if (Source.GetLength() != 0)
						return true;
				}

				return false;
			};
			preprocessParameters.Defines = Info->Defines;
			preprocessorParser.Process(preprocessParameters);

			FrameAllocator parserAllocator("Program Parser Allocator", RenderSystemAllocators::ProgramCompilerAllocator);
			Parser parser(&parserAllocator, preprocessParameters.Result);
			Parser::Parameters parameters;
			parser.Parse(parameters);

			FrameAllocator compilerAllocator("Program APICompiler Allocator", RenderSystemAllocators::ProgramCompilerAllocator);

			if (!parameters.Functions.ContainsIf([](auto& item) { return item->GetType() != FunctionType::Types::None; }))
				return;

			for (uint8 i = 0; i < TypeCount; ++i)
			{
				compilerAllocator.Reset();

				DeviceTypes deviceType = Types[i];

				OutputInfo& output = Outputs[i];

				String moduleName = "";
				switch (deviceType)
				{
				case DeviceTypes::OpenGL:
				case DeviceTypes::Vulkan:
					moduleName = "ASTToGLSLCompiler";
					break;

				case DeviceTypes::DirectX12:
					moduleName = "ASTToHLSLCompiler";
					break;

				default:
					CoreDebugAssert(Categories::ProgramCompiler, false, "Device type is not supported");
				}

				ASTCompilerBase* compiler = ModuleManager::GetInstance()->Load<ASTCompilerBase>(moduleName);
				CoreDebugAssert(Categories::ProgramCompiler, compiler != nullptr, "Couldn't load %s module", moduleName);

				compiler->Initialize(&compilerAllocator, deviceType);
				compiler->Compile(parameters.Structs, parameters.Variables, parameters.Functions, output);

				for (auto& structType : parameters.Structs)
				{
					auto variables = structType->GetItems();
					variables.RemoveIf([](auto item) { return item->GetRegister().GetLength() != 0; });

					if (variables.GetSize() == 0)
						continue;

					output.MetaInfo.Structs.Add({});
					StructMetaInfo& structMeta = output.MetaInfo.Structs[output.MetaInfo.Structs.GetSize() - 1];

					structMeta.Name = structType->GetName();

					for (auto& variableType : variables)
					{
						ProgramDataTypes dataType = variableType->GetDataType()->GetType();

						structMeta.Variables.Add({ dataType, variableType->GetName() });
					}

					structMeta.Size = StructType::GetStructSize(structType);
				}

				uint8 bindingCount = 0;
				for (auto& variableType : parameters.Variables)
				{
					output.MetaInfo.Variables.Add({});
					VariableMetaInfo& variableMeta = output.MetaInfo.Variables[output.MetaInfo.Variables.GetSize() - 1];

					DataTypeStatement* type = variableType->GetDataType();

					variableMeta.Handle = bindingCount++;
					variableMeta.Name = variableType->GetName();
					variableMeta.DataType = type->GetType();
					variableMeta.UserDefinedType = type->GetUserDefined();

					if (deviceType != DeviceTypes::OpenGL && type->GetType() == ProgramDataTypes::Texture2D)
						++bindingCount;
				}
			}
		}
	}
}
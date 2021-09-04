// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\CompilerHelper.h>
#include <Rendering\Private\ProgramCompiler\Compiler.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\DirectX12\DirectX12Device.h>
#include <Rendering\Private\Vulkan\VulkanDevice.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			using namespace OpenGL;
			using namespace DirectX12;
			using namespace Vulkan;

			namespace ProgramCompiler
			{
				//https://github.com/KhronosGroup/Vulkan-Docs/issues/274
				bool CompilerHelper::Compile(const ProgramInfo& Info, const DeviceTypes* DeviceTypes, uint8 DeviceTypeCount, CompiledProgramInfo* CompiledInfos)
				{
					if (Info.Source.GetLength() == 0)
						return false;

					CompileOutputInfo outputInfos[DEVICE_TYPE_COUNT];
					try
					{
						Compiler::GetInstance()->Compile(&Info, DeviceTypes, DeviceTypeCount, outputInfos);
					}
					catch (const Exception& ex)
					{
						for (uint8 i = 0; i < DeviceTypeCount; ++i)
						{
							CompiledProgramInfo& compiledProgrm = CompiledInfos[i];

							compiledProgrm.VertexShader.Size = 0;
							compiledProgrm.TessellationShader.Size = 0;
							compiledProgrm.GeometryShader.Size = 0;
							compiledProgrm.FragmentShader.Size = 0;
							compiledProgrm.ComputeShader.Size = 0;

							compiledProgrm.ErrorMessage = ex.ToString();
						}

						return false;
					}

					for (uint8 i = 0; i < DeviceTypeCount; ++i)
					{
						if (outputInfos->VertexShader == String::Empty &&
							outputInfos->TessellationShader == String::Empty &&
							outputInfos->GeometryShader == String::Empty &&
							outputInfos->FragmentShader == String::Empty &&
							outputInfos->ComputeShader == String::Empty)
							continue;

						CompileOutputInfo& outputInfo = outputInfos[i];
						CompiledProgramInfo& compiledProgrm = CompiledInfos[i];

						IDevice::Shaders shaders = {};
						shaders.VertexShader = outputInfo.VertexShader.GetValue();
						shaders.TessellationShader = outputInfo.TessellationShader.GetValue();
						shaders.GeometryShader = outputInfo.GeometryShader.GetValue();
						shaders.FragmentShader = outputInfo.FragmentShader.GetValue();
						shaders.ComputeShader = outputInfo.ComputeShader.GetValue();

						IDevice::CompiledShaders compiledShaders = {};
						compiledShaders.VertexShader.Buffer = compiledProgrm.VertexShader.Buffer;
						compiledShaders.VertexShader.Size = compiledProgrm.VertexShader.Size;
						compiledShaders.TessellationShader.Buffer = compiledProgrm.TessellationShader.Buffer;
						compiledShaders.TessellationShader.Size = compiledProgrm.TessellationShader.Size;
						compiledShaders.GeometryShader.Buffer = compiledProgrm.GeometryShader.Buffer;
						compiledShaders.GeometryShader.Size = compiledProgrm.GeometryShader.Size;
						compiledShaders.FragmentShader.Buffer = compiledProgrm.FragmentShader.Buffer;
						compiledShaders.FragmentShader.Size = compiledProgrm.FragmentShader.Size;
						compiledShaders.ComputeShader.Buffer = compiledProgrm.ComputeShader.Buffer;
						compiledShaders.ComputeShader.Size = compiledProgrm.ComputeShader.Size;

						cstr message = nullptr;
						bool result = false;

						switch (DeviceTypes[i])
						{
						case DeviceTypes::OpenGL:
							result = OpenGLDevice::CompileProgramAPI(&shaders, &compiledShaders, &message);
							break;

						case DeviceTypes::DirectX12:
							result = DirectX12Device::CompileProgramAPI(&shaders, &compiledShaders, &message);
							break;

						case DeviceTypes::Vulkan:
							result = VulkanDevice::CompileProgramAPI(&shaders, &compiledShaders, &message);
							break;
						}

						CoreDebugAssert(Categories::ProgramCompiler, result, "It seems that an invalid shader tries to get compiled in an API {%s}", message);

						compiledProgrm.VertexShader.Size = compiledShaders.VertexShader.Size;
						compiledProgrm.TessellationShader.Size = compiledShaders.TessellationShader.Size;
						compiledProgrm.GeometryShader.Size = compiledShaders.GeometryShader.Size;
						compiledProgrm.FragmentShader.Size = compiledShaders.FragmentShader.Size;
						compiledProgrm.ComputeShader.Size = compiledShaders.ComputeShader.Size;

						compiledProgrm.MetaInfo = outputInfo.MetaInfo;
					}

					return true;
				}
			}
		}
	}
}
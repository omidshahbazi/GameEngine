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
				bool CompilerHelper::Compile(const ProgramInfo& Info, const DeviceTypes* DeviceTypes, uint8 DeviceTypeCount, CompiledProgramInfo* CompiledInfos, Compiler::ErrorFunction OnError)
				{
					if (Info.Source.GetLength() == 0)
						return false;

					ProgramInfo info = {};
					//info.Source =
					//	"struct INPUT_DATA { float3 pos : POSITION; float3 col : UV; };"
					//	"struct DATA { matrix4 _MVP;  matrix4 _View; float time; texture2D tex; };"
					//	"DATA data;"
					//	"float4 VertexMain(INPUT_DATA InputData)"
					//	"{"
					//	"	return data._MVP * data._View * float4(InputData.pos, 1);"
					//	"}"
					//	"float4 FragmentMain(INPUT_DATA InputData)"
					//	"{"
					//	"	return float4(InputData.col, data.time);"
					//	"}";
					//info.Source =
					//	"struct INPUT_DATA { float3 pos : POSITION; float3 col : UV; };"
					//	"struct DATA { float time; matrix4 _MVP;  matrix4 _View; };"
					//	"DATA data;"
					//	"float4 VertexMain(INPUT_DATA InputData)"
					//	"{"
					//	"	return data._MVP * data._View * float4(InputData.pos, 1);"
					//	"}"
					//	"float4 FragmentMain(INPUT_DATA InputData)"
					//	"{"
					//	"	return float4(InputData.col, data.time);"
					//	"}";
					//info.Source =
					//	"struct INPUT_DATA { float3 pos : POSITION; float3 col : UV; };"
					//	"struct DATA { float time; matrix4 _MVP;  matrix4 _View; };"
					//	"DATA data;"
					//	"float4 VertexMain(INPUT_DATA InputData)"
					//	"{"
					//	"	return float4(InputData.pos, 1);"
					//	"}"
					//	"float4 FragmentMain(INPUT_DATA InputData)"
					//	"{"
					//	"	return float4(1, 0, 1, 1);"
					//	"}";

					Compiler::OutputInfo outputInfos[4];
					if (!Compiler::GetInstance()->Compile(&Info, DeviceTypes, DeviceTypeCount, outputInfos, OnError))
						return false;

					for (uint8 i = 0; i < DeviceTypeCount; ++i)
					{
						Compiler::OutputInfo& outputInfo = outputInfos[i];

						IDevice::Shaders shaders = {};
						shaders.VertexShader = outputInfo.VertexShader.GetValue();
						shaders.TessellationShader = outputInfo.TessellationShader.GetValue();
						shaders.GeometryShader = outputInfo.GeometryShader.GetValue();
						shaders.FragmentShader = outputInfo.FragmentShader.GetValue();
						shaders.ComputeShader = outputInfo.ComputeShader.GetValue();

						CompiledProgramInfo& compiledProgrm = CompiledInfos[i];

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
							result = true;
							//result = DirectX12Device::CompileProgramAPI(&shaders, &compiledShaders, &message);
							break;

						case DeviceTypes::Vulkan:
							result = VulkanDevice::CompileProgramAPI(&shaders, &compiledShaders, &message);
							break;
						}

						if (!result)
						{
							if (OnError != nullptr)
								OnError(message, 0);

							return false;
						}

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
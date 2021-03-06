// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILER_H
#define COMPILER_H

#include <Rendering\RenderingCommon.h>
#include <Rendering\ProgramInfo.h>
#include <Rendering\ProgramDataTypes.h>
#include <MemoryManagement\Singleton.h>
#include <Containers\Strings.h>
#include <Containers\ListenerContainer.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				class RENDERING_API Compiler
				{
				public:
					static cstr ENTRY_POINT_NAME;

					typedef std::function<void(const String& Message, uint16 Line)> ErrorFunction;

					struct VariableMetaInfo
					{
					public:
						String DataType;
						String Name;
					};

					struct StructMetaInfo
					{
						struct VariableMetaInfo
						{
						public:
							ProgramDataTypes DataType;
							String Name;
						};

					public:
						String Name;
						Vector<VariableMetaInfo> Variables;
					};

					struct MetaInfo
					{
					public:
						Vector<StructMetaInfo> Structs;

						Vector<VariableMetaInfo> Variables;
					};

					struct OutputInfo
					{
					public:
						String VertexShader;
						String GeometryShader;
						String DomainShader;
						String FragmentShader;
						String ComputeShader;

						MetaInfo* MetaInfo;
					};

				public:
					class RENDERING_API IListener
					{
					public:
						virtual bool FetchShaderSource(const String& Name, String& Source)
						{
							return false;
						}
					};

					SINGLETON_DECLARATION(Compiler);

					LISTENER_DECLARATION(IListener);

				private:
					Compiler(void)
					{
					}

				public:
					bool Compile(DeviceTypes DeviceType, const ProgramInfo* Info, OutputInfo &Output, ErrorFunction OnError = nullptr);
				};
			}
		}
	}
}

#endif
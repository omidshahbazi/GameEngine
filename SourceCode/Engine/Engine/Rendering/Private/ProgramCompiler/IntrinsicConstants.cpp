// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\IntrinsicConstants.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				using namespace Syntax;

				IntrinsicConstants::IntrinsicConstants(DeviceTypes Device)
				{
#define BEGIN_CONSTANT(Name, DataType) \
					{ \
						const String name = Name; \
						Assert(!m_Constants.Contains(name), "Constant already added"); \
						ConstantInfo& info = m_Constants[name]; \
						info.Type = DataType;

#define BEGIN_DESCRIPTION(DeviceType, BuildCallback) \
						if (Device == DeviceType) \
							info.Build = std::make_shared<ConstantInfo::BuildStatement>(BuildCallback);

#define END_CONSTANT() \
					}

					BEGIN_CONSTANT("_FragPosition", ProgramDataTypes::Float2);
					{
						BEGIN_DESCRIPTION(DeviceTypes::OpenGL, [this](auto Type, auto Stage, auto& Shader)
							{
								if (Type != FunctionType::Types::FragmentMain)
									return;

								Shader += "gl_FragCoord.xy";
							});
						BEGIN_DESCRIPTION(DeviceTypes::DirectX12, [this](auto Type, auto Stage, auto& Shader)
							{
								if (Type != FunctionType::Types::FragmentMain)
									return;

								InjectParameterIntoTopFunction(ProgramDataTypes::Float4, "dx_FragCoord", "SV_Position");

								Shader += "dx_FragCoord.xy";
							});
					}
					END_CONSTANT();

#undef BEGIN_CONSTANT
#undef BEGIN_DESCRIPTION
#undef END_CONSTANT
				}

				bool IntrinsicConstants::BuildIntrinsicConstantStatement(const String& Name, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					if (!m_Constants.Contains(Name))
						return false;

					auto& constant = m_Constants[Name];

					(*constant.Build)(Type, Stage, Shader);

					return true;
				}
			}
		}
	}
}
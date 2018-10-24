// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TYPE_H
#define TYPE_H

#include <Rendering\Private\ShaderCompiler\DataTypes.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class Type
				{
				public:
					Type(void)
					{
					}

					void SetName(const String &Name)
					{
						m_Name = Name;
					}

					const String &GetName(void) const
					{
						return m_Name;
					}

					virtual String ToString(void) const = 0;

				protected:
					static String GetDataTypeName(DataTypes Type)
					{
						switch (Type)
						{
						case DataTypes::Void:
							return "void";

						case DataTypes::Float:
							return "float";

						case DataTypes::Float2:
							return "float2";

						case DataTypes::Float3:
							return "float3";

						case DataTypes::Float4:
							return "float4";

						case DataTypes::Matrix4:
							return "matrix4";
						}

						return "";
					}

				private:
					String m_Name;
				};
			}
		}
	}
}

#endif
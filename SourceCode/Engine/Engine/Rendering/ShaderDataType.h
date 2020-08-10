// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_DATA_TYPE_H
#define SHADER_DATA_TYPE_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class ShaderDataType
		{
		public:
			enum class Types
			{
				Void = 0,
				Bool,
				Float,
				Double,
				Float2,
				Double2,
				Float3,
				Double3,
				Float4,
				Double4,
				Matrix4,
				Texture2D,
				Unknown
			};

		public:
			ShaderDataType(void) :
				m_Type(Types::Unknown),
				m_ElementCount(0)
			{
			}

			ShaderDataType(Types Type) :
				m_Type(Type),
				m_ElementCount(1)
			{
			}

			ShaderDataType(Types Type, uint8 ElementCount) :
				m_Type(Type),
				m_ElementCount(ElementCount)
			{
			}

			Types GetType(void) const
			{
				return m_Type;
			}

			uint8 GetElementCount(void) const
			{
				return m_ElementCount;
			}

			String ToString(void) const
			{
				switch (m_Type)
				{
				case Types::Void:
					return "void";

				case Types::Bool:
					return "bool";

				case Types::Float:
					return "float";

				case Types::Double:
					return "double";

				case Types::Float2:
					return "float2";

				case Types::Double2:
					return "double2";

				case Types::Float3:
					return "float3";

				case Types::Double3:
					return "double3";

				case Types::Float4:
					return "float4";

				case Types::Double4:
					return "double4";

				case Types::Matrix4:
					return "matrix4";
				}

				return "";
			}

		private:
			Types m_Type;
			uint8 m_ElementCount;
		};
	}
}

#endif
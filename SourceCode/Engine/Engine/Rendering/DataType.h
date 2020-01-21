// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class DataType
		{
		public:
			enum class Types
			{
				Void = 0,
				Float,
				Float2,
				Float3,
				Float4,
				Matrix4,
				Texture2D,
				Unknown
			};

		public:
			DataType(void) :
				m_Type(Types::Unknown),
				m_ElementCount(0)
			{
			}

			DataType(Types Type) :
				m_Type(Type),
				m_ElementCount(1)
			{
			}

			DataType(Types Type, uint8 ElementCount) :
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

				case Types::Float:
					return "float";

				case Types::Float2:
					return "float2";

				case Types::Float3:
					return "float3";

				case Types::Float4:
					return "float4";

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
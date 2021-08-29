// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DATA_TYPE_STATEMENT_H
#define DATA_TYPE_STATEMENT_H

#include <Rendering\Private\ProgramCompiler\Syntax\Statement.h>
#include <Rendering\ProgramDataTypes.h>
#include <Containers\Strings.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
				{
					class DataTypeStatement : public Statement
					{
					public:
						DataTypeStatement(void) :
							m_Type(ProgramDataTypes::Unknown),
							m_ElementCount(nullptr)
						{
						}

						DataTypeStatement(ProgramDataTypes Type) :
							m_Type(Type),
							m_ElementCount(nullptr)
						{
						}

						DataTypeStatement(const String& UserDefined) :
							m_Type(ProgramDataTypes::Unknown),
							m_UserDefined(UserDefined),
							m_ElementCount(nullptr)
						{
						}

						DataTypeStatement(ProgramDataTypes Type, Statement* ElementCountStatement) :
							m_Type(Type),
							m_ElementCount(ElementCountStatement)
						{
						}

						DataTypeStatement(const String& UserDefined, Statement* ElementCountStatement) :
							m_Type(ProgramDataTypes::Unknown),
							m_UserDefined(UserDefined),
							m_ElementCount(ElementCountStatement)
						{
						}

						ProgramDataTypes GetType(void) const
						{
							return m_Type;
						}

						const String& GetUserDefined(void) const
						{
							return m_UserDefined;
						}

						Statement* GetElementCount(void) const
						{
							return m_ElementCount;
						}

						bool IsBuiltIn(void) const
						{
							return (m_Type != ProgramDataTypes::Unknown);
						}

						//bool IsUnrecognized(void) const
						//{
						//	return (!IsBuiltIn() && m_UserDefined.GetLength() == 0);
						//}

						String ToString(void) const
						{
							String result = "";

							switch (m_Type)
							{
							case ProgramDataTypes::Void:
								result = "void";
								break;

							case ProgramDataTypes::Bool:
								result = "bool";
								break;

							case ProgramDataTypes::Float:
								result = "float";
								break;

							case ProgramDataTypes::Double:
								result = "double";
								break;

							case ProgramDataTypes::Float2:
								result = "float2";
								break;

							case ProgramDataTypes::Double2:
								result = "double2";
								break;

							case ProgramDataTypes::Float3:
								result = "float3";
								break;

							case ProgramDataTypes::Double3:
								result = "double3";
								break;

							case ProgramDataTypes::Float4:
								result = "float4";
								break;

							case ProgramDataTypes::Double4:
								result = "double4";
								break;

							case ProgramDataTypes::Matrix4:
								result = "matrix4";
								break;

							default:
								result = m_UserDefined;
								break;
							}

							if (m_ElementCount != nullptr)
							{
								result += "[";
								result += m_ElementCount->ToString();
								result += "]";
							}

							return result;
						}

					private:
						ProgramDataTypes m_Type;
						String m_UserDefined;
						Statement* m_ElementCount;
					};
				}
			}
		}
	}
}

#endif
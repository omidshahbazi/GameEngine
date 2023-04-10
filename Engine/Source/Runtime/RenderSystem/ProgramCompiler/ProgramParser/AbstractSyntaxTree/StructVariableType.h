// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRUCT_VARIABLE_TYPE_H
#define STRUCT_VARIABLE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\VariableType.h>
#include <ProgramParser\AbstractSyntaxTree\StructType.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class StructType;

			class PROGRAMPARSER_API StructVariableType : public VariableType
			{
			public:
				enum class Registers
				{
					None = 0,
					Position,
					Normal,
					Color,
					UV,
					TessellationFactor,
					InsideTessellationFactor,
					PrimitiveID,
					DomainLocation,
					InstanceID,
					OutputControlPointID,
					FragmentPosition,
					Target,
					DispatchThreadID,
					GroupID,
					GroupIndex,
					GroupThreadID
				};

			public:
				StructVariableType(StructType* Parent, AllocatorBase* Allocator) :
					VariableType(Allocator),
					m_Parent(Parent),
					m_Register(Registers::None),
					m_RegisterIndex(0)
				{
				}

				const StructType* GetParent(void) const
				{
					return m_Parent;
				}

				Registers GetRegister(void) const
				{
					return m_Register;
				}

				String GetRegisterName(void) const
				{
					return GetRegisterName(m_Register);
				}

				void SetRegister(Registers Register)
				{
					m_Register = Register;
				}

				uint8 GetRegisterIndex(void) const
				{
					return m_RegisterIndex;
				}

				void SetRegisterIndex(uint8 Index)
				{
					m_RegisterIndex = Index;
				}

				virtual String ToString(void) const override
				{
					String result;

					result += GetDataType()->ToString();
					result += ' ';
					result += GetName();
					result += GetDataType()->PostElementCountToString();

					if (m_Register != Registers::None)
					{
						result += " : ";
						result += GetRegisterName();
						result += StringUtility::ToString<char8>(m_RegisterIndex);
					}

					result += ";";

					return result;
				}

				static String GetRegisterName(Registers Register)
				{
					switch (Register)
					{
					case Registers::Position:
						return "POSITION";

					case Registers::Normal:
						return "NORMAL";

					case Registers::Color:
						return "COLOR";

					case Registers::UV:
						return "UV";

					case Registers::PrimitiveID:
						return "PRIMITIVE_ID";

					case Registers::TessellationFactor:
						return "TESSELLATION_FACTOR";

					case Registers::InsideTessellationFactor:
						return "INSIDE_TESSELLATION_FACTOR";

					case Registers::DomainLocation:
						return "DOMAIN_LOCATION";

					case Registers::InstanceID:
						return "INSTANCE_ID";

					case Registers::OutputControlPointID:
						return "OUTPUT_CONTROL_POINT_ID";

					case Registers::FragmentPosition:
						return "FRAGMENT_POSITION";

					case Registers::Target:
						return "TARGET";

					case Registers::DispatchThreadID:
						return "DISPATCH_THREAD_ID";

					case Registers::GroupID:
						return "GROUP_ID";

					case Registers::GroupIndex:
						return "GROUP_INDEX";

					case Registers::GroupThreadID:
						return "GROUP_THREAD_ID";
					}

					THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
				}

				static ProgramDataTypes GetRegisterDataType(Registers Register)
				{
					switch (Register)
					{
					case Registers::Position:
					case Registers::Normal:
					case Registers::Color:
					case Registers::UV:
						return ProgramDataTypes::Unknown;

					case Registers::PrimitiveID:
						return ProgramDataTypes::Integer;

					case Registers::TessellationFactor:
						return ProgramDataTypes::Float;

					case Registers::InsideTessellationFactor:
						return ProgramDataTypes::Float;

					case Registers::DomainLocation:
						return ProgramDataTypes::Float3;

					case Registers::InstanceID:
						return ProgramDataTypes::Integer;

					case Registers::OutputControlPointID:
						return ProgramDataTypes::Integer;

					case Registers::FragmentPosition:
						return ProgramDataTypes::Float4;

					case Registers::Target:
						return ProgramDataTypes::Float4;

					case Registers::DispatchThreadID:
						return ProgramDataTypes::UnsignedInteger3;

					case Registers::GroupID:
						return ProgramDataTypes::UnsignedInteger3;

					case Registers::GroupIndex:
						return ProgramDataTypes::UnsignedInteger;

					case Registers::GroupThreadID:
						return ProgramDataTypes::UnsignedInteger3;
					}

					THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
				}

			private:
				StructType* m_Parent;
				Registers m_Register;
				uint8 m_RegisterIndex;
			};

			typedef Vector<StructVariableType*> StructVariableList;
		}
	}
}

#endif
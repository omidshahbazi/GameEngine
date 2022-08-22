// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRUCT_VARIABLE_TYPE_H
#define STRUCT_VARIABLE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\VariableType.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
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
					DomainLocation,
					InstanceID,
					FragmentPosition,
					Target,
					DispatchThreadID,
					GroupID,
					GroupIndex,
					GroupThreadID
				};

			public:
				StructVariableType(AllocatorBase* Allocator) :
					VariableType(Allocator),
					m_Register(Registers::None),
					m_RegisterIndex(0)
				{
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

					result += GetDataType()->ToString() + " " + GetName();

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

					case Registers::DomainLocation:
						return "DOMAIN_LOCATION";

					case Registers::InstanceID:
						return "INSTANCE_ID";

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
				}

			private:
				Registers m_Register;
				uint8 m_RegisterIndex;
			};

			typedef Vector<StructVariableType*> StructVariableList;
		}
	}
}

#endif
// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef REFLECTION_GENERATOR_H
#define REFLECTION_GENERATOR_H
#include <Reflection\Private\ImplementDataStructureType.h>
#include <Reflection\Private\ImplementEnumType.h>
#include <Reflection\Private\ImplementPropertyType.h>
#include <ReflectionTool\MetaDataStructure.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Reflection;
	using namespace Containers;

	namespace ReflectionTool
	{
		class REFLECTIONTOOL_API ReflectionGenerator
		{
		public:
			ReflectionGenerator(const String &FilePath, const String &OutputBaseFileName) :
				m_FilePath(FilePath),
				m_OutputBaseFileName(OutputBaseFileName)
			{
			}

			bool Generate(void);

		private:
			void GenerateHeaderFile(String &HeaderContent, const TypesList &Types);
			void GenerateCompileFile(String &CompileContent, const TypesList &Types);
			void GenerateDataStructuresDefinition(String &RootContent, String &Content, String &FunctionsDefinition, const TypesList &Types, AccessSpecifiers Access);
			void GenerateConstructorsDefinition(String &Content, const TypesList &Types, AccessSpecifiers Access);
			void GenerateFunctionsDefinition(String &Content, const TypesList &Types, AccessSpecifiers Access);
			void GenerateVariablesDefinition(String &Content, const TypesList &Types, AccessSpecifiers Access);

			static String GetPointerName(Type *Type)
			{
				return (Type->GetType() == Type::Types::DataStructure ? ((MetaDataStructure*)Type)->GetUniqueName() : Type->GetName()) + "Ptr";
			}

			static String GetAccessText(AccessSpecifiers Access)
			{
				return (Access == AccessSpecifiers::Public ? "Public" : "Private");
			}

			INLINE static String GetArgumentsText(const Parameter::ParametersList &Parameters)
			{
				String ret;

				if (Parameters.GetSize() != 0)
				{
					for (uint8 i = 0; i < Parameters.GetSize(); i++)
					{
						const Parameter &param = Parameters[i];

						ret += GetArgumentText(i, param.GetDataType());

						if (i < Parameters.GetSize() - 1)
							ret += ",";
					}
				}

				return ret;
			}

			INLINE static String GetArgumentText(int32 Index, const DataType &Type)
			{
				String ret;

				if (Type.GetValueType() == ValueTypes::None)
					if (Type.GetPassType() == DataType::PassesTypes::Pointer)
						ret += "(" + Type.GetExtraValueType() + "*)";

				ret += "(*Arguments)[" + StringUtility::ToString<char8>(Index) + "].GetAs";

				if (Type.GetPassType() == DataType::PassesTypes::Pointer)
					ret += GetValueTypeText(ValueTypes::VoidPointer);
				else
					ret += GetValueTypeText(Type.GetValueType(), false);

				ret += "()";

				return ret;
			}

			INLINE static String GetValueTypeText(ValueTypes Type, bool WithPrefix = false)
			{
				String ret = (WithPrefix ? "ValueTypes::" : "");

				switch (Type)
				{
				case ValueTypes::VoidPointer:
					ret += "VoidPointer";
					break;

				case ValueTypes::Bool:
					ret += "Bool";
					break;

				case ValueTypes::Int8:
					ret += "Int8";
					break;
				case ValueTypes::Int16:
					ret += "Int16";
					break;
				case ValueTypes::Int32:
					ret += "Int32";
					break;
				case ValueTypes::Int64:
					ret += "Int64";
					break;

				case ValueTypes::UInt8:
					ret += "UInt8";
					break;
				case ValueTypes::UInt16:
					ret += "UInt16";
					break;
				case ValueTypes::UInt32:
					ret += "UInt32";
					break;
				case ValueTypes::UInt64:
					ret += "UInt64";
					break;

				case ValueTypes::Float32:
					ret += "Float32";
					break;
				case ValueTypes::Float64:
					ret += "Float64";
					break;

				case ValueTypes::String:
					ret += "String";
					break;

				case ValueTypes::WString:
					ret += "WString";
					break;
				}

				return ret;
			}

		private:
			String m_FilePath;
			String m_OutputBaseFileName;
			String m_OutputClassName;
		};

	}
}
#endif

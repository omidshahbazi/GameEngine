// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\Private\HeaderFileGenerator.h>

namespace Engine
{
	using namespace Reflection;

	namespace ReflectionGenerator
	{
		using namespace Private;

		void HeaderFileGenerator::Generate(const TypeList& Types, const WString& OutputFilePath)
		{
			String content = FILE_HEADER;

			GenerateHeaderCode(Types, content);

			WriteToFile(OutputFilePath, content);
		}

		void HeaderFileGenerator::GenerateHeaderCode(const TypeList& Types, String& Content)
		{
#ifdef DEBUG_MODE
#define ADD_NEW_LINE() Content += "\n";
#else
#define ADD_NEW_LINE()
#endif

			const String OBJECTTYPE(STRINGIZE(Engine::Reflection::ObjectType));

			for (auto& t : Types)
			{
				Content += "\n";

				if (IsTypeOf(t, ObjectType))
				{
					MetaObject* type = ReinterpretCast(MetaObject*, t);

					const String macroName = type->GetDeclarationMacroName();
					const String reflectionClassName = GetUniqueClassName(type);

					Content += R"(#include <Reflection\Definitions.h>)";
					ADD_NEW_LINE();

					Content += R"(#include <Reflection\DataStructureType.h>)";
					ADD_NEW_LINE();

					Content += "using namespace ;";
					ADD_NEW_LINE();

					Content += "#ifdef " + macroName;
					ADD_NEW_LINE();
					Content += "#undef " + macroName;
					ADD_NEW_LINE();
					Content += "#endif";
					ADD_NEW_LINE();
					Content += "#define " + macroName + "() \\";
					ADD_NEW_LINE();
					Content += "	friend class " + reflectionClassName + "; \\";
					ADD_NEW_LINE();
					Content += "public: \\";
					ADD_NEW_LINE();
					Content += "		static const " + OBJECTTYPE + "& GetType(void);";

					TypeList nestedTypes;
					type->GetNestedTypes(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, nestedTypes);
					GenerateHeaderCode(nestedTypes, Content);
				}
			}

#undef ADD_NEW_LINE
		}
	}
}

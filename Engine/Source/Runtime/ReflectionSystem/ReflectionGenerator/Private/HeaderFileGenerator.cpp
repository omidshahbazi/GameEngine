// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\Private\HeaderFileGenerator.h>
#include <functional>

namespace Engine
{
	using namespace Reflection;

	namespace ReflectionGenerator
	{
#ifdef DEBUG_MODE
#define ADD_NEW_LINE() Content += "\n";
#else
#define ADD_NEW_LINE()
#endif

		using namespace Private;

		void HeaderFileGenerator::Generate(const TypeList& Types, const WString& FilePath, const WString& OutputFilePath)
		{
			String content = FILE_HEADER;

			GenerateHeaderCode(Types, content);
			GenerateTypesCode(Types, content);

			WriteToFile(OutputFilePath, content);
		}

		void HeaderFileGenerator::GenerateHeaderCode(const TypeList& Types, String& Content)
		{
			ADD_NEW_LINE();

			Content += R"(#include <Reflection\Definitions.h>)";
			ADD_NEW_LINE();

			Content += R"(#include <Reflection\ObjectType.h>)";
			ADD_NEW_LINE();

			ADD_NEW_LINE();
		}

		void HeaderFileGenerator::GenerateTypesCode(const TypeList& Types, String& Content)
		{
			for (auto& t : Types)
				GenerateTypeCode(t, Content);
		}

		void HeaderFileGenerator::GenerateTypeCode(Type* Type, String& Content)
		{
			if (IsTypeOf(Type, ObjectType))
				GenerateObjectCode(ReinterpretCast(MetaObject*, Type), Content);
		}

		void HeaderFileGenerator::GenerateObjectCode(MetaObject* Type, String& Content)
		{
			const String OBJECT_TYPE(STRINGIZE(Engine::Reflection::ObjectType));
			const String MACRO_NAME = Type->GetDeclarationMacroName();
			const String IMPLEMENT_OBJECT_TYPE = GetImplementType(Type);

			Content += "#ifdef " + MACRO_NAME;
			ADD_NEW_LINE();
			Content += "#undef " + MACRO_NAME;
			ADD_NEW_LINE();
			Content += "#endif";
			ADD_NEW_LINE();
			Content += "#define " + MACRO_NAME + "() \\";
			ADD_NEW_LINE();
			Content += "	friend class " + IMPLEMENT_OBJECT_TYPE + "; \\";
			ADD_NEW_LINE();

			std::function<void(MetaObject*, AccessSpecifiers)> generateFriendClassForNesteds = [&generateFriendClassForNesteds, &Content](MetaObject* Type, AccessSpecifiers AccessLevel)
			{
				TypeList items;
				Type->GetNestedTypes(AccessLevel, items);

				for (auto& item : items)
				{
					const String IMPLEMENT_NESTED_TYPE = GetImplementType(item);

					Content += "	friend class " + IMPLEMENT_NESTED_TYPE + "; \\";
					ADD_NEW_LINE();

					if (IsTypeOf(item, MetaObject))
						generateFriendClassForNesteds(ReinterpretCast(MetaObject*, item), AccessLevel);
				}
			};

			std::function<void(MetaObject*, AccessSpecifiers)> generateFriendClassForFunctions = [&generateFriendClassForFunctions, &Content](MetaObject* Type, AccessSpecifiers AccessLevel)
			{
				FunctionTypeList items;
				Type->GetFunctions(AccessLevel, items);

				for (auto& item : items)
				{
					const String IMPLEMENT_FUNCTION_TYPE = GetImplementType(item);

					Content += "	friend class " + IMPLEMENT_FUNCTION_TYPE + "; \\";
					ADD_NEW_LINE();
				}

				TypeList nestedTypes;
				Type->GetNestedTypes(AccessLevel, nestedTypes);

				for (auto& item : nestedTypes)
					if (IsTypeOf(item, MetaObject))
						generateFriendClassForFunctions(ReinterpretCast(MetaObject*, item), AccessLevel);
			};

			std::function<void(MetaObject*, AccessSpecifiers)> generateFriendClassForNProperties = [&generateFriendClassForNProperties, &Content](MetaObject* Type, AccessSpecifiers AccessLevel)
			{
				PropertyTypeList items;
				Type->GetProperties(AccessLevel, items);

				for (auto& item : items)
				{
					const String IMPLEMENT_PROPERTY_TYPE = GetImplementType(item);

					Content += "	friend class " + IMPLEMENT_PROPERTY_TYPE + "; \\";
					ADD_NEW_LINE();
				}

				TypeList nestedTypes;
				Type->GetNestedTypes(AccessLevel, nestedTypes);

				for (auto& item : nestedTypes)
					if (IsTypeOf(item, MetaObject))
						generateFriendClassForNProperties(ReinterpretCast(MetaObject*, item), AccessLevel);
			};

			generateFriendClassForNesteds(Type, AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public);
			generateFriendClassForFunctions(Type, AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public);
			generateFriendClassForNProperties(Type, AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public);

			Content += "public: \\";
			ADD_NEW_LINE();

#define IMPLEMENT_GET_FUNCTION(Name) \
			Content += "const " + OBJECT_TYPE + "& " + Name + "(void); \\"; \
			ADD_NEW_LINE();

			Content += "		virtual ";
			IMPLEMENT_GET_FUNCTION(GET_MEMBER_FUNCTION_NAME);

			Content += "		static ";
			IMPLEMENT_GET_FUNCTION(GET_STATIC_FUNCTION_NAME);

#undef IMPLEMENT_GET_FUNCTION

			ADD_NEW_LINE();

			TypeList nestedTypes;
			Type->GetNestedTypes(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, nestedTypes);
			GenerateTypesCode(nestedTypes, Content);

#undef ADD_NEW_LINE
		}
	}
}

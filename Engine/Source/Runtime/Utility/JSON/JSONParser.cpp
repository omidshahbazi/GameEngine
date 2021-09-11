// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <JSON\JSONParser.h>
#include <JSON\JSONObject.h>
#include <JSON\JSONArray.h>
#include <JSON\JSONArray.h>
#include <JSON\JSONException.h>
#include <Lexer\Tokenizer.h>

namespace Engine
{
	using namespace Lexer;

	namespace JSON
	{
		class JSONTokenizer : public Tokenizer
		{
		public:
			JSONTokenizer(const String& Value) :
				Tokenizer(Value)
			{
			}

		public:
			bool ThrowException()
			{
				Token token;
				if (Tokenizer::GetToken(token))
					THROW_JSON_EXCEPTION(StringUtility::Format(String("Unexpected token %s in %l"), token.GetIdentifier(), token.GetLineIndex()), m_CurrentLineIndex);

				THROW_JSON_EXCEPTION("Unexpected end of stream", m_CurrentLineIndex);
			}

			bool MatchASymbol(const String& Symbol)
			{
				return Tokenizer::MatchSymbol(Symbol);
			}

			bool MatchAnIdentifier(const String& Symbol)
			{
				return Tokenizer::MatchIdentifier(Symbol);
			}

			void RequireASymbol(const String& Symbol, const String& Tag = String::Empty)
			{
				Tokenizer::RequireSymbol(Symbol, Tag);
			}

			void RequireALiteral(String& Literal)
			{
				Token token;
				if (!GetToken(token))
					ThrowException();

				if (token.GetTokenType() != Token::Types::Constant || token.GetConstantString() == String::Empty)
				{
					UngetToken(token);
					ThrowException();
				}

				Literal = token.GetConstantString();
			}

			void RequireAConstant(Token& Token)
			{
				if (!GetToken(Token))
					ThrowException();

				if (Token.GetTokenType() != Token::Types::Constant)
				{
					UngetToken(Token);
					ThrowException();
				}
			}
		};

		void ParseObject(AllocatorBase* Allocator, JSONTokenizer* Tokenizer, JSONObject* Object);
		void ParseArray(AllocatorBase* Allocator, JSONTokenizer* Tokenizer, JSONArray* Array);

		void ParseBasic(AllocatorBase* Allocator, JSONTokenizer* Tokenizer, JSONBasic* Basic)
		{
			if (Tokenizer->MatchASymbol(OPEN_BRACKET))
			{
				if (!IsAssignableFrom(Basic, JSONObject))
					THROW_JSON_EXCEPTION("Provided Basic parameter is Array, but Object is needed", 0);

				ParseObject(Allocator, Tokenizer, ReinterpretCast(JSONObject*, Basic));

				return;
			}

			if (Tokenizer->MatchASymbol(OPEN_SQUARE_BRACKET))
			{
				if (!IsAssignableFrom(Basic, JSONArray))
					THROW_JSON_EXCEPTION("Provided Basic parameter is Object, but Array is needed", 0);

				ParseArray(Allocator, Tokenizer, ReinterpretCast(JSONArray*, Basic));

				return;
			}

			Tokenizer->ThrowException();
		}

		void ParseData(AllocatorBase* Allocator, JSONTokenizer* Tokenizer, JSONData* Data)
		{
			if (Tokenizer->MatchASymbol(OPEN_BRACKET))
			{
				JSONObject* obj = ReinterpretCast(JSONObject*, AllocateMemory(Allocator, sizeof(JSONObject)));
				Construct(obj, Allocator);

				ParseObject(Allocator, Tokenizer, obj);

				*Data = obj;

				return;
			}

			if (Tokenizer->MatchASymbol(OPEN_SQUARE_BRACKET))
			{
				JSONArray* arr = ReinterpretCast(JSONArray*, AllocateMemory(Allocator, sizeof(JSONArray)));
				Construct(arr, Allocator);

				ParseArray(Allocator, Tokenizer, arr);

				*Data = arr;

				return;
			}

			if (Tokenizer->MatchAnIdentifier("null"))
			{
				*Data = AnyDataType(nullptr);

				return;
			}

			Token token;
			Tokenizer->RequireAConstant(token);

			const String& value = token.GetIdentifier();

			if (value == "true")
				*Data = true;
			else if (value == "false")
				*Data = AnyDataType(false);
			else if (value.Contains('.'))
				*Data = StringUtility::ToFloat64(value);
			else if (CharacterUtility::IsDigit(value.GetValue()))
				*Data = StringUtility::ToInt64(value);
			else if (token.GetTokenType() == Token::Types::Constant)
				*Data = String(Allocator, token.GetConstantString());
			else
				Tokenizer->ThrowException();
		}

		void ParseObject(AllocatorBase* Allocator, JSONTokenizer* Tokenizer, JSONObject* Object)
		{
			while (true)
			{
				String key(Allocator);
				Tokenizer->RequireALiteral(key);

				Tokenizer->RequireASymbol(COLON, "Object parsing");

				JSONData data;
				ParseData(Allocator, Tokenizer, &data);

				(*Object)[key] = data;

				if (Tokenizer->MatchASymbol(CLOSE_BRACKET))
					break;

				Tokenizer->RequireASymbol(COMMA, "Object parsing");
			}
		}

		void ParseArray(AllocatorBase* Allocator, JSONTokenizer* Tokenizer, JSONArray* Array)
		{
			while (true)
			{
				JSONData data;
				ParseData(Allocator, Tokenizer, &data);

				Array->Add(data);

				if (Tokenizer->MatchASymbol(CLOSE_SQUARE_BRACKET))
					break;

				Tokenizer->RequireASymbol(COMMA, "Array parsing");
			}
		}

		void JSONParser::Parse(AllocatorBase* Allocator, const String& Value, JSONBasic* Basic)
		{
			JSONTokenizer tokenizer(Value);
			tokenizer.Parse();

			ParseBasic(Allocator, &tokenizer, Basic);
		}
	}
}
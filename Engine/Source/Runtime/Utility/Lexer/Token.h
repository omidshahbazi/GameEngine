// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef TOKEN_H
#define TOKEN_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Lexer
	{
		class LEXER_API Token
		{
		public:
			enum class Types
			{
				None = 0,
				Identifier,
				Symbol,
				Constant
			};

			enum class SearchCases
			{
				IgnoreCase = 0,
				CaseSensitive
			};

		public:
			Token(void);
			Token(const Token& Token);
			~Token(void);

			bool Matches(const String& Name) const;
			bool Matches(const String& Value, SearchCases SearchCase) const;

			INLINE bool GetConstantBool(void) const
			{
				return m_Bool;
			}

			INLINE int32 GetConstantInt32(void) const
			{
				return m_Int32;
			}

			INLINE float32 GetConstantFloat32(void) const
			{
				return m_Float32;
			}

			INLINE const float64& GetConstantFloat64(void) const
			{
				return m_Float64;
			}

			INLINE const String& GetConstantString(void) const
			{
				return m_String;
			}

			INLINE Types GetTokenType(void) const
			{
				return m_Type;
			}

			INLINE const String& GetName(void) const
			{
				return m_Name;
			}

			INLINE const String& GetIdentifier(void) const
			{
				return m_Identifier;
			}

			INLINE uint32 GetStartIndex(void) const
			{
				return m_StartIndex;
			}

			INLINE uint32 GetLineIndex(void) const
			{
				return m_LineIndex;
			}

			INLINE uint32 GetColumnIndex(void) const
			{
				return m_ColumnIndex;
			}

			Token& operator=(const Token& Token);

			INLINE String& GetIdentifier(void)
			{
				return m_Identifier;
			}

			INLINE void SetConstantBool(bool Value)
			{
				m_Bool = Value;
				m_Type = Types::Constant;
			}

			INLINE void SetConstantInt32(int32 Value)
			{
				m_Int32 = Value;
				m_Type = Types::Constant;
			}

			INLINE void SetConstantFloat32(float32 Value)
			{
				m_Float32 = Value;
				m_Type = Types::Constant;
			}

			INLINE void SetConstantFloat64(float64 Value)
			{
				m_Float64 = Value;
				m_Type = Types::Constant;
			}

			INLINE void SetConstantString(const String& Value)
			{
				m_String = Value;
				m_Type = Types::Constant;
			}

			INLINE void SetType(Types Value)
			{
				m_Type = Value;
			}

			INLINE void SetName(const String& Value)
			{
				m_Name = Value;
			}

			INLINE void SetIdentifier(const String& Value)
			{
				m_Identifier = Value;
			}

			INLINE void SetStartIndex(uint32 Value)
			{
				m_StartIndex = Value;
			}

			INLINE void SetLineIndex(uint32 Value)
			{
				m_LineIndex = Value;
			}

			INLINE void SetColumnIndex(uint32 Value)
			{
				m_ColumnIndex = Value;
			}

		private:
			Types m_Type;

			String m_Name;
			String m_Identifier;

			uint32 m_StartIndex;
			uint32 m_LineIndex;
			uint32 m_ColumnIndex;

			union
			{
				bool m_Bool;

				int32 m_Int32;

				float32 m_Float32;
				float64 m_Float64;
			};

			String m_String;
		};
	}
}
#endif

// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef TOKEN_H
#define TOKEN_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Utility
	{
		namespace Lexer
		{
			class UTILITY_API Token
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

				Token(const Token &Token);

				~Token(void)
				{
				}

				bool Matches(const String &Name) const;
				bool Matches(const String &Value, SearchCases SearchCase) const;

				bool GetConstantBool(void) const
				{
					return m_Bool;
				}
				void SetConstantBool(bool Value);

				int32 GetConstantInt32(void)const
				{
					return m_Int32;
				}
				void SetConstantInt32(int32 Value);

				float32 GetConstantFloat32(void)const
				{
					return m_Float32;
				}
				void SetConstantFloat32(float32 Value);

				const float64 &GetConstantFloat64(void) const
				{
					return m_Float64;
				}
				void SetConstantFloat64(float64 Value);

				const String &GetConstantString(void) const
				{
					return m_String;
				}
				void SetConstantString(const String &Value);

				INLINE Types GetTokenType(void) const
				{
					return m_Type;
				}
				INLINE void SetType(Types Value)
				{
					m_Type = Value;
				}

				INLINE String &GetName(void)
				{
					return m_Name;
				}
				INLINE const String &GetName(void) const
				{
					return m_Name;
				}
				INLINE void SetName(const String &Value)
				{
					m_Name = Value;
				}

				INLINE String &GetIdentifier(void)
				{
					return m_Identifier;
				}
				INLINE const String &GetIdentifier(void) const
				{
					return m_Identifier;
				}
				INLINE void SetIdentifier(const String &Value)
				{
					m_Identifier = Value;
				}

				INLINE uint32 GetStartIndex(void) const
				{
					return m_StartIndex;
				}
				INLINE void SetStartIndex(uint32 Value)
				{
					m_StartIndex = Value;
				}

				INLINE uint32 GetLineIndex(void) const
				{
					return m_LineIndex;
				}
				INLINE void SetLineIndex(uint32 Value)
				{
					m_LineIndex = Value;
				}

				Token &operator =(const Token &Token);

			private:
				Types m_Type;

				String m_Name;
				String m_Identifier;

				uint32 m_StartIndex;
				uint32 m_LineIndex;

				union
				{
					bool m_Bool;

					int32 m_Int32;

					float32 m_Float32;
					float64 m_Float64;

					String m_String;
				};
			};
		}
	}
}
#endif

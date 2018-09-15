// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef TOKEN_H
#define TOKEN_H

#include <Reflection\PropertyType.h>

namespace Engine
{
	using namespace Reflection;

	namespace ReflectionTool
	{
		class Token : public PropertyType
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
			~Token(void)
			{
			}

			bool Matches(const String &Name) const;
			bool Matches(const String &Value, SearchCases SearchCase) const;

			void SetConstantBool(bool Value);
			void SetConstantInt32(int32 Value);
			void SetConstantFloat32(float32 Value);
			void SetConstantFloat64(float64 Value);
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

#endif

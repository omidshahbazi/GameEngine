// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONSTANT_DATA_H
#define CONSTANT_DATA_H

#include <RenderCommon\RenderCommon.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace RenderCommon;
	using namespace Containers;

	namespace RenderSystem
	{
		namespace Private
		{
			template<typename T>
			struct ConstantData
			{
			public:
				ConstantData(void) :
					Handle(0)
				{
				}

				ConstantData(ProgramConstantHandle Handle, const T& Value) :
					Handle(Handle),
					Value(Value)
				{
				}

				INLINE ConstantData& operator =(const ConstantData& Other)
				{
					Handle = Other.Handle;
					Value = Other.Value;

					return *this;
				}

			public:
				ProgramConstantHandle Handle;
				T Value;
			};

			template<typename T>
			struct MetaConstantData : public ConstantData<T>
			{
			public:
				MetaConstantData(void) :
					Hash(0)
				{
				}

				MetaConstantData(ProgramConstantHandle Handle, const String& Name, const T& Value) :
					ConstantDataBase<T>(Handle, Value),
					Name(Name),
					Hash(GetHash(Name))
				{
				}

				MetaConstantData(ProgramConstantHandle Handle, const String& Name, const String& UserDefinedType, const T& Value) :
					ConstantDataBase<T>(Handle, Value),
					Name(Name),
					Hash(GetHash(Name)),
					UserDefinedType(UserDefinedType)
				{
				}

				INLINE ConstantData& operator =(const ConstantData& Other)
				{
					ConstantData<T>::operator=(Other);

					Name = Other.Name;
					Hash = Other.Hash;
					UserDefinedType = Other.UserDefinedType;

					return *this;
				}

			public:
				String Name;
				ProgramConstantHash Hash;
				String UserDefinedType;
			};
		}
	}
}

#endif
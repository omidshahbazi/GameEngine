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
					Handle(0),
					Value(nullptr)
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

				MetaConstantData(ProgramConstantHandle Handle, const String& Name, ProgramConstantHash Hash, const T& Value) :
					ConstantData<T>(Handle, Value),
					Name(Name),
					Hash(Hash)
				{
				}

				MetaConstantData(ProgramConstantHandle Handle, const String& Name, ProgramConstantHash Hash, const String& UserDefinedType, const T& Value) :
					ConstantData<T>(Handle, Value),
					Name(Name),
					Hash(Hash),
					UserDefinedType(UserDefinedType)
				{
				}

				INLINE MetaConstantData& operator =(const MetaConstantData& Other)
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
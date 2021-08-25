// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

#include <Common\PrimitiveTypes.h>
#include <functional>

namespace Engine
{
	namespace Common
	{
		template<typename ClassType, typename DataType> constexpr uint32 OffsetOf(DataType ClassType::* Member)
		{
			return ReinterpretCast(uint8*, &(ReinterpretCast(ClassType*, nullptr)->*Member)) - ReinterpretCast(uint8*, nullptr);
		}

		struct DefaultType
		{
		public:
			template<typename T>
			operator T() const
			{
				return T();
			}
		};

		DefaultType const default_value = DefaultType();

		template <typename T> struct FunctionType;

		template <typename ClassType, typename ReturnType, typename... ParameterTypes>
		struct FunctionType<ReturnType(ClassType::*)(ParameterTypes...)>
		{
		public:
			typedef std::function<ReturnType(ParameterTypes...)> Type;
		};

		template <typename ReturnType, typename... ParameterTypes>
		struct FunctionType<ReturnType(*)(ParameterTypes...)>
		{
		public:
			typedef std::function<ReturnType(ParameterTypes...)> Type;
		};

		template<typename ReturnType, typename ClassType, typename... ParameterTypes>
		std::function<ReturnType(ParameterTypes...)> Attach(ReturnType(ClassType::* Function)(ParameterTypes...), ClassType* Instance)
		{
			return [Function, Instance](ParameterTypes... args)->ReturnType { return (Instance->*Function)(args...); };
		};
	}
}

#endif
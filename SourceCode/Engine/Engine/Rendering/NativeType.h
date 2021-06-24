// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef NATIVE_TYPE_H
#define NATIVE_TYPE_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			class ThreadedDevice;

			namespace Commands
			{
				class DrawCommandBase;
			}
		}

		using namespace Private;
		using namespace Private::Commands;

		class NativeType
		{
			friend class DrawCommandBase;
			friend class Program;
			friend class ProgramConstantSupplier;
			friend class DeviceInterface;

		public:
#if X64
			typedef uint64 Handle;
#else
			typedef uint32 Handle;
#endif

		public:
			NativeType(ThreadedDevice* Device, Handle Handle) :
				m_Device(Device),
				m_Handle(Handle)
			{
			}

			const WString& GetName(void)
			{
				return m_Name;
			}

			virtual void SetName(const WString& Name)
			{
				m_Name = Name;
			}

		protected:
			INLINE ThreadedDevice* GetDevice(void) const
			{
				return m_Device;
			}

			INLINE Handle GetHandle(void) const
			{
				return m_Handle;
			}

		private:
			ThreadedDevice* m_Device;
			Handle m_Handle;
			WString m_Name;
		};
	}
}

#endif
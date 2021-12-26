// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef NATIVE_TYPE_H
#define NATIVE_TYPE_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <RenderCommon\RenderCommon.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace RenderCommon;

	namespace RenderSystem
	{
		namespace Private
		{
			class ThreadedDevice;

			namespace Commands
			{
				class DrawCommand;
			}
		}

		using namespace Private;
		using namespace Private::Commands;

		class NativeType
		{
			friend class CommandBuffer;
			friend class DrawCommand;
			friend class Program;
			friend class ProgramConstantSupplier;
			friend class DeviceInterface;

		public:
			NativeType(ThreadedDevice* Device, ResourceHandle Handle) :
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

			INLINE ResourceHandle GetHandle(void) const
			{
				return m_Handle;
			}

		private:
			ThreadedDevice* m_Device;
			ResourceHandle m_Handle;
			WString m_Name;
		};
	}
}

#endif
// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUILT_IN_PROGRAM_CONSTANT_SUPPLIER_H
#define BUILT_IN_PROGRAM_CONSTANT_SUPPLIER_H

#include <MemoryManagement\Singleton.h>
#include <Rendering\DeviceInterface.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			class BuiltiInProgramConstantSupplier : private DeviceInterface::IListener
			{
				SINGLETON_DECLARATION(BuiltiInProgramConstantSupplier);

				friend class DeviceInterface;
				friend class DrawCommand;

			private:
				BuiltiInProgramConstantSupplier(void) :
					m_DeviceInterface(nullptr)
				{
				}

				~BuiltiInProgramConstantSupplier(void);

				void Initialize(DeviceInterface* DeviceInterface);

			private:
				void OnWindowChanged(Window* Window) override;
				void OnWindowResized(Window* Window) override;

			private:
				DeviceInterface* m_DeviceInterface;
				Vector2I m_FrameSize;
			};
		}
	}
}

#endif
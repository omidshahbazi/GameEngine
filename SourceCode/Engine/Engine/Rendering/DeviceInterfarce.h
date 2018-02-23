// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef RENDERING_INTERFACE_H
#define RENDERING_INTERFACE_H

namespace Engine
{
	namespace Rendering
	{
		class IDevice;

		class RENDERING_API DeviceInterfarce
		{
		public:
			enum class Type
			{
				OpenGL
			};

		public:
			DeviceInterfarce(Type Type);
			
		private:
			void InitializeDevice(void);

		private:
			Type m_Type;
			IDevice *m_Device;
		};
	}
}

#endif